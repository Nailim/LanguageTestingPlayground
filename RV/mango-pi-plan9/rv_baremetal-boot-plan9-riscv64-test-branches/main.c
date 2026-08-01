
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler/assembler test: hand-written branch instructions (BEQ/BNE/
 * BLT/BGE/BLTU/BGEU, both the 2-operand-vs-zero and 3-operand forms)
 * and the "unconditional JMP back to a label preceded by a conditional
 * branch" loop idiom. See README for what this test targets and why.
 */

#define GPIO_BASE 0x02000000
#define PD_CFG2   (GPIO_BASE + 0x0098)
#define PD_DATA   (GPIO_BASE + 0x00A0)
#define LED_PIN        18
#define LED_BIT        (1 << LED_PIN)
#define LED_CFG_SHIFT  ((LED_PIN - 16) * 4)
#define LED_CFG_MASK   (0xF << LED_CFG_SHIFT)
#define LED_CFG_OUTPUT (0x1 << LED_CFG_SHIFT)

#define UART0_BASE 0x02500000
#define UART_THR (UART0_BASE + (0 * 4))
#define UART_LSR (UART0_BASE + (5 * 4))
#define UART_LSR_THRE (1 << 5)

void gpio_led_init(void)
{
	unsigned int cfg;

	cfg = GET32(PD_CFG2);
	cfg &= ~LED_CFG_MASK;
	cfg |= LED_CFG_OUTPUT;
	PUT32(PD_CFG2, cfg);
}

void gpio_led_set(int on)
{
	unsigned int data;

	data = GET32(PD_DATA);
	if(on)
		data |= LED_BIT;
	else
		data &= ~LED_BIT;
	PUT32(PD_DATA, data);
}

void uart_putc(int c)
{
	while(!(GET32(UART_LSR) & UART_LSR_THRE))
		;
	PUT32(UART_THR, c & 0xFF);
}

void uart_puts(char *s)
{
	while(*s){
		if(*s == '\n')
			uart_putc('\r');
		uart_putc(*s++);
	}
}

void delay(unsigned int count)
{
	unsigned int i;

	for(i = 0; i < count; i++)
		dummy(i);
}

int failed;

void check(char *label, int got, int expect)
{
	uart_puts(label);
	if(got == expect)
		uart_puts(" PASS\n");
	else{
		uart_puts(" FAIL\n");
		failed++;
	}
}

/* declared in branchtest.s - see that file for what each one does and
 * why its particular operand values were chosen */
int beq_true(void);
int beq_false(void);
int bne_true(void);
int bne_false(void);

int blt_basic(void);
int blt_sign(void);
int blt_sign_rev(void);
int bge_basic(void);
int bge_sign(void);
int bge_sign_rev(void);
int bltu_basic(void);
int bltu_sign(void);
int bltu_sign_rev(void);
int bgeu_basic(void);
int bgeu_sign(void);
int bgeu_sign_rev(void);

int beqz_true(void);
int beqz_false(void);
int bnez_true(void);
int bnez_false(void);
int bltz_true(void);
int bltz_false(void);
int bgez_true(void);
int bgez_false(void);

int loop_topguard_count(void);
int loop_bottomtest_count(void);

int main(void)
{
	gpio_led_init();
	uart_puts("test-branches starting\n");

	failed = 0;

	check("beq_true",  beq_true(),  1);
	check("beq_false", beq_false(), 0);
	check("bne_true",  bne_true(),  1);
	check("bne_false", bne_false(), 0);

	check("blt_basic",    blt_basic(),    1);
	check("blt_sign",     blt_sign(),     1);
	check("blt_sign_rev", blt_sign_rev(), 0);
	check("bge_basic",    bge_basic(),    1);
	check("bge_sign",     bge_sign(),     1);
	check("bge_sign_rev", bge_sign_rev(), 0);
	check("bltu_basic",    bltu_basic(),    1);
	check("bltu_sign",     bltu_sign(),     1);
	check("bltu_sign_rev", bltu_sign_rev(), 0);
	check("bgeu_basic",    bgeu_basic(),    1);
	check("bgeu_sign",     bgeu_sign(),     1);
	check("bgeu_sign_rev", bgeu_sign_rev(), 0);

	check("beqz_true",  beqz_true(),  1);
	check("beqz_false", beqz_false(), 0);
	check("bnez_true",  bnez_true(),  1);
	check("bnez_false", bnez_false(), 0);
	check("bltz_true",  bltz_true(),  1);
	check("bltz_false", bltz_false(), 0);
	check("bgez_true",  bgez_true(),  1);
	check("bgez_false", bgez_false(), 0);

	/* loop_topguard_count's guard uses 3-operand BGE, which the a.y fix
	 * above also covers - its old expected value (0) was calibrated to
	 * the PRE-fix swapped behavior and is now stale. Testing the
	 * naively-correct count (5) here to find out empirically whether
	 * the separate "JMP back to a label preceded by a conditional
	 * branch compiles into a different conditional branch" mystery
	 * from the original l.s bug is also resolved, or still a live,
	 * separate issue - see branchtest.s and PLAN9_RISCV64_LESSONS_LEARNED.md. */
	check("loop_topguard_count (post-fix re-check)", loop_topguard_count(), 5);
	check("loop_bottomtest_count (fixed shape)",      loop_bottomtest_count(), 7);

	if(failed == 0)
		uart_puts("ALL TESTS PASSED\n");
	else
		uart_puts("SOME TESTS FAILED (see above)\n");

	/* heartbeat: solid LED = done, all passed. Fast blink = done,
	 * at least one FAIL was printed above. */
	while(1){
		if(failed == 0){
			gpio_led_set(1);
		}else{
			gpio_led_set(1);
			delay(2000000);
			gpio_led_set(0);
			delay(2000000);
		}
	}
	return(0);
}
