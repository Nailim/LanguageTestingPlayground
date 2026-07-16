
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: pointer arithmetic / array indexing scaled by element
 * width. See README for what this test targets and why.
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

/* Global (data-segment) int array - width 4, so every step needs to be
 * scaled by 4 bytes, unlike uart_puts's char* (width 1, where a
 * scaling bug would be invisible - see plan9-riscv-missing-setsb, found
 * on this exact kind of small-global address materialization). */
int iarr[6] = {10, 20, 30, 40, 50, 60};

int main(void)
{
	int *p, *q, *end;
	int sum;
	char buf[4];
	char *ptrs[4];

	gpio_led_init();
	uart_puts("test-arrayidx starting\n");

	failed = 0;

	/* [] indexing on a global int array */
	check("iarr[0]", iarr[0], 10);
	check("iarr[3]", iarr[3], 40);
	check("iarr[5]", iarr[5], 60);

	/* pointer arithmetic: +, ++, -- all need a scale-by-4, not
	 * scale-by-1 like the already-proven char* case */
	p = iarr;
	check("*p initial", *p, 10);
	p = p + 3;
	check("*(p+3)", *p, 40);
	p++;
	check("*p after ++", *p, 50);
	p--;
	p--;
	check("*p after two --", *p, 30);

	/* pointer difference is in element units (C semantics), not
	 * bytes - another place a wrong scale factor would show up */
	check("(iarr+5)-iarr", (int)((iarr + 5) - iarr), 5);
	check("(iarr+5)-(iarr+2)", (int)((iarr + 5) - (iarr + 2)), 3);

	/* write through a pointer, then confirm via [] on the same
	 * underlying array - checks both directions agree */
	p = iarr + 2;
	*p = 999;
	check("iarr[2] after *p=999", iarr[2], 999);

	/* local (stack-relative, frame-pointer-addressed) array - a
	 * different addressing mode from iarr's global/data-segment
	 * addressing, so this is new coverage, not a repeat of the
	 * checks above */
	buf[0] = 'A';
	buf[1] = 'B';
	buf[2] = 'C';
	buf[3] = 'D';
	check("buf[0]", buf[0], 'A');
	check("buf[3]", buf[3], 'D');

	/* array of pointers - each element is 8 bytes (TIND width) on
	 * riscv64, a third distinct scale factor from char*(1) and
	 * int*(4) above */
	ptrs[0] = &buf[0];
	ptrs[1] = &buf[1];
	ptrs[2] = &buf[2];
	ptrs[3] = &buf[3];
	check("*ptrs[0]", *ptrs[0], 'A');
	check("*ptrs[3]", *ptrs[3], 'D');
	check("ptrs[3]-ptrs[0]", (int)(ptrs[3] - ptrs[0]), 3);

	/* pointer relational compare in a loop condition, iterating the
	 * whole (now partially mutated) array */
	sum = 0;
	q = iarr;
	end = iarr + 6;
	while(q < end){
		sum += *q;
		q++;
	}
	check("sum via pointer-compare loop", sum, 10 + 20 + 999 + 40 + 50 + 60);

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
