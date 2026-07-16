
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: signed/unsigned division and modulo.
 * See README for what this test targets and why.
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

/*
 * Real function calls (not compile-time-foldable constants) so the
 * compiler is forced to emit an actual runtime DIVW/DIVUW/REMW/REMUW
 * instruction rather than folding the whole expression at compile time.
 */
int val(int x)
{
	return x;
}

unsigned int uval(unsigned int x)
{
	return x;
}

int main(void)
{
	gpio_led_init();
	uart_puts("test-divmod starting\n");

	failed = 0;

	/* signed division/modulo: every sign combination, C truncates
	 * toward zero (matches RISC-V DIV/REM semantics directly) */
	check("17/5 div",   val(17)  / val(5),   3);
	check("17/5 mod",   val(17)  % val(5),   2);
	check("-17/5 div",  val(-17) / val(5),  -3);
	check("-17/5 mod",  val(-17) % val(5),  -2);
	check("17/-5 div",  val(17)  / val(-5), -3);
	check("17/-5 mod",  val(17)  % val(-5),  2);
	check("-17/-5 div", val(-17) / val(-5),  3);
	check("-17/-5 mod", val(-17) % val(-5), -2);
	check("100/7 div",  val(100) / val(7),  14);
	check("100/7 mod",  val(100) % val(7),   2);
	check("7/100 div",  val(7)   / val(100), 0);
	check("7/100 mod",  val(7)   % val(100), 7);
	check("-1/2 div",   val(-1)  / val(2),   0);
	check("-1/2 mod",   val(-1)  % val(2),  -1);

	/* unsigned division/modulo: operands that look negative if
	 * misinterpreted as signed, to catch a signed/unsigned mixup */
	check("0xFFFFFFF0u/16u div", (int)(uval(0xFFFFFFF0u) / uval(16u)), (int)0x0FFFFFFFu);
	check("0xFFFFFFF0u/16u mod", (int)(uval(0xFFFFFFF0u) % uval(16u)), 0);
	check("0x80000000u/2u div",  (int)(uval(0x80000000u) / uval(2u)),  (int)0x40000000u);
	check("0x80000000u/2u mod",  (int)(uval(0x80000000u) % uval(2u)),  0);
	check("0xFFFFFFFFu/10u div", (int)(uval(0xFFFFFFFFu) / uval(10u)), (int)429496729u);
	check("0xFFFFFFFFu/10u mod", (int)(uval(0xFFFFFFFFu) % uval(10u)), 5);
	check("10u/0xFFFFFFFFu div", (int)(uval(10u) / uval(0xFFFFFFFFu)), 0);
	check("10u/0xFFFFFFFFu mod", (int)(uval(10u) % uval(0xFFFFFFFFu)), 10);

	/* RISC-V M-extension spec-defined overflow: INT_MIN/-1 does not
	 * trap or match ISO C (which calls this UB) - the ISA defines
	 * DIV to return the dividend and REM to return 0. Worth checking
	 * this custom core (T-Head C906) actually follows the spec. */
	check("INT_MIN/-1 div", val(-2147483647 - 1) / val(-1), -2147483647 - 1);
	check("INT_MIN/-1 mod", val(-2147483647 - 1) % val(-1), 0);

	/* RISC-V M-extension spec-defined divide-by-zero: no trap.
	 * DIV by 0 returns -1, DIVU by 0 returns all-ones, REM/REMU by 0
	 * return the dividend unchanged. */
	check("5/0 div",   val(5) / val(0),  -1);
	check("5/0 mod",   val(5) % val(0),   5);
	check("5u/0u div", (int)(uval(5u) / uval(0u)), -1); /* all-ones */
	check("5u/0u mod", (int)(uval(5u) % uval(0u)),  5);

	if(failed == 0)
		uart_puts("ALL TESTS PASSED\n");
	else
		uart_puts("SOME TESTS FAILED (see above)\n");

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
