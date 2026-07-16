
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: indirect calls through a function pointer.
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

/* Four targets, same signature, deliberately different bodies so a
 * misdirected call (landing on the wrong function, or on garbage) is
 * very likely to produce a wrong, checkable result rather than an
 * accidental match. */
int add10(int x)
{
	return x + 10;
}

int mul2(int x)
{
	return x * 2;
}

int neg(int x)
{
	return -x;
}

int square(int x)
{
	return x * x;
}

typedef int (*fn1)(int);

/* Function pointer passed as an argument - a second, distinct code path
 * from a function pointer held in a local/global variable: the callee
 * has to receive the pointer value itself (not the pointee) and call
 * through it. */
int apply(fn1 f, int x)
{
	return f(x);
}

int main(void)
{
	fn1 p;

	gpio_led_init();
	uart_puts("test-funcptr starting\n");

	failed = 0;

	/* function pointer held in a local variable, reassigned between
	 * calls - each call must dispatch to whichever function p
	 * currently holds, not get stuck on the first one */
	p = add10;
	check("p==add10", (p == add10), 1);
	check("p=add10, p(5)", p(5), 15);

	p = mul2;
	check("p==mul2", (p == mul2), 1);
	check("p=mul2, p(5)", p(5), 10);

	p = neg;
	check("p==neg", (p == neg), 1);
	check("p=neg, p(5)", p(5), -5);

	p = square;
	check("p==square", (p == square), 1);
	check("p=square, p(5)", p(5), 25);

	/* function pointer passed as an argument */
	check("apply(add10,7)",  apply(add10,  7),  17);
	check("apply(mul2,7)",   apply(mul2,   7),  14);
	check("apply(neg,7)",    apply(neg,    7),  -7);
	check("apply(square,7)", apply(square, 7),  49);

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
