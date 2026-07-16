
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: 64-bit (long long / unsigned long long) arithmetic
 * across the 32-bit boundary. See README for what this test targets
 * and why.
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

void check64(char *label, long long got, long long expect)
{
	uart_puts(label);
	if(got == expect)
		uart_puts(" PASS\n");
	else{
		uart_puts(" FAIL\n");
		failed++;
	}
}

/* Real function calls (not compile-time-foldable constants), same
 * reasoning as val()/uval() in test-divmod - forces genuine runtime
 * 64-bit ADD/SUB/MUL/shift instructions instead of a compile-time
 * constant. Every interesting 64-bit value below is built at runtime
 * from these small (safely-in-32-bit-range) seeds rather than written
 * as a large literal in source - sys/src/cmd/cc's own literal lexer
 * wasn't available to inspect in this session, so this sidesteps any
 * question about its 64-bit-literal handling entirely and keeps this
 * test isolated to arithmetic codegen specifically. */
long long vid(long long x)
{
	return x;
}

unsigned long long uvid(unsigned long long x)
{
	return x;
}

int main(void)
{
	long long one, big, altbig, a, b;
	unsigned long long ubig;

	gpio_led_init();
	uart_puts("test-longmath starting\n");

	failed = 0;

	one = vid(1);
	big = one << 32;             /* 0x100000000, via a single shift-by-32 */
	altbig = (one << 16) << 16;  /* same target, via two shift-by-16 - an
	                               * independent instruction sequence, in
	                               * case exactly-32 is special-cased */
	check64("1<<32 == (1<<16)<<16", big, altbig);

	a = big - one;                /* 0xFFFFFFFF */
	check64("(1<<32)-1, +1 carries back to 1<<32", a + one, big);

	/* relational compares where a hypothetical bug that only compared
	 * the low 32 bits would give the WRONG answer (not coincidentally
	 * agree with the correct 64-bit answer) */
	check64("1<<32 > 0xFFFFFFFF", (big > a) ? 1 : 0, 1);
	check64("1<<32 < 1 is false", (big < one) ? 1 : 0, 0);

	b = vid(100000) * vid(100000); /* 10,000,000,000 - overflows 32 bits */
	check64("100000*100000 == 100000*50000*2", b,
		(vid(100000) * vid(50000)) * vid(2));

	check64("1<<40 == (1<<20)<<20", one << 40, (one << 20) << 20);

	ubig = uvid((unsigned long long)1 << 63); /* top bit only */
	check64("(1u<<63) > 0 unsigned", (ubig > 0) ? 1 : 0, 1);
	check64("(1u<<63) as signed is negative", (((long long)ubig) < 0) ? 1 : 0, 1);

	check64("big+3 < big+10", (big + vid(3) < big + vid(10)) ? 1 : 0, 1);

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
