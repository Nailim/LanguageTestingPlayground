
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler/assembler test: CSR writes, bulk register save/restore, and
 * non-commutative 3-operand arithmetic - the three things flagged as
 * needed-but-untested before Phase 2 (trap handling) starts leaning on
 * them. See README for what each section targets and why.
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

void uart_putdec(int n)
{
	char buf[8];
	int i;

	if(n == 0){
		uart_putc('0');
		return;
	}
	i = 0;
	while(n > 0 && i < 8){
		buf[i++] = '0' + (n % 10);
		n /= 10;
	}
	while(i > 0)
		uart_putc(buf[--i]);
}

void delay(unsigned int count)
{
	unsigned int i;

	for(i = 0; i < count; i++)
		dummy(i);
}

int failed;

void check(char *label, long long got, long long expect)
{
	uart_puts(label);
	if(got == expect)
		uart_puts(" PASS\n");
	else{
		uart_puts(" FAIL\n");
		failed++;
	}
}

/* csrtest.s */
long long csr_sscratch_zero(void);
long long csr_sscratch_allones(void);
long long csr_sscratch_pattern(void);
long long csr_stvec_roundtrip(void);
long long csr_sepc_roundtrip(void);

/* regtest.s */
void regsave_test(void);
extern unsigned long long regbuf1[32];
extern unsigned long long regbuf2[32];

/* arithtest.s */
long long sub_test(void);
long long slt_true(void);
long long slt_false(void);
long long sltu_true(void);
long long sltu_false(void);
long long sltu_sign(void);
long long sll_test(void);
long long srl_test(void);
long long sra_test(void);

void check_regbuf(char *label, unsigned long long *buf, int comparetoself)
{
	int i, ok, firstbad;
	unsigned long long expect;

	ok = 1;
	firstbad = -1;
	for(i = 5; i <= 31; i++){
		if(comparetoself)
			expect = regbuf1[i];
		else
			expect = -(unsigned long long)i;
		if(buf[i] != expect){
			ok = 0;
			if(firstbad < 0)
				firstbad = i;
		}
	}
	uart_puts(label);
	if(ok)
		uart_puts(" PASS\n");
	else{
		uart_puts(" FAIL (first bad: R");
		uart_putdec(firstbad);
		uart_puts(")\n");
		failed++;
	}
}

int main(void)
{
	gpio_led_init();
	uart_puts("test-trapprep starting\n");

	failed = 0;

	uart_puts("-- CSR writes --\n");
	check("csr_sscratch_zero",     csr_sscratch_zero(),     0);
	check("csr_sscratch_allones",  csr_sscratch_allones(), -1);
	check("csr_sscratch_pattern",  csr_sscratch_pattern(), 305);
	check("csr_stvec_roundtrip",   csr_stvec_roundtrip(),  0x100);
	check("csr_sepc_roundtrip",    csr_sepc_roundtrip(),   0x200);

	uart_puts("-- register save/restore --\n");
	regsave_test();
	check_regbuf("regbuf1 (save matches assigned pattern)", regbuf1, 0);
	check_regbuf("regbuf2 (restore+resave matches regbuf1)", regbuf2, 1);

	uart_puts("-- non-commutative 3-operand arithmetic --\n");
	check("sub_test",   sub_test(),   7);
	check("slt_true",   slt_true(),   1);
	check("slt_false",  slt_false(),  0);
	check("sltu_true",  sltu_true(),  1);
	check("sltu_false", sltu_false(), 0);
	check("sltu_sign",  sltu_sign(),  1);
	check("sll_test",   sll_test(),   16);
	check("srl_test",   srl_test(),   15);
	check("sra_test",   sra_test(),   -4);

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
