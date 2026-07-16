
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: switch/case dispatch.
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
 * Deliberately non-contiguous case values (so the compiler can't turn
 * this into a trivial array lookup) and 9 cases, enough to force swt.c's
 * swit2() past its "nc < 5" linear-chain threshold into at least one
 * level of its binary-search bisection (see swt.c: it picks the middle
 * case, compares OGT/OEQ against it, and recurses into both halves -
 * this is NOT a jump table, just a tree of compare+branch instructions).
 */
int classify(int v)
{
	switch(v){
	case 1:
		return 100;
	case 2:
		return 200;
	case 3:
		return 300;
	case 5:
		return 500;
	case 8:
		return 800;
	case 13:
		return 1300;
	case 21:
		return 2100;
	case 34:
		return 3400;
	case 55:
		return 5500;
	default:
		return -1;
	}
}

int main(void)
{
	gpio_led_init();
	uart_puts("test-switch starting\n");

	failed = 0;

	/* every case value, in a different order than declared - checks
	 * the binary-search dispatch isn't relying on call order */
	check("v=21", classify(21), 2100);
	check("v=1",  classify(1),  100);
	check("v=55", classify(55), 5500);
	check("v=3",  classify(3),  300);
	check("v=8",  classify(8),  800);
	check("v=2",  classify(2),  200);
	check("v=34", classify(34), 3400);
	check("v=5",  classify(5),  500);
	check("v=13", classify(13), 1300);

	/* values that hit default: below the lowest case, above the
	 * highest case, and in gaps between existing cases */
	check("v=0 (default, below all)",     classify(0),   -1);
	check("v=4 (default, gap 3..5)",      classify(4),   -1);
	check("v=9 (default, gap 8..13)",     classify(9),   -1);
	check("v=14 (default, gap 13..21)",   classify(14),  -1);
	check("v=100 (default, above all)",   classify(100), -1);

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
