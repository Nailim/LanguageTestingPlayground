
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: struct field access/padding, assignment, pass-by-
 * pointer, pass-by-value, and return-by-value. See README for what
 * this test targets and why.
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

/* char/int/char deliberately mixes a 4-byte-aligned member between two
 * 1-byte members, the classic padding-prone shape. This test doesn't
 * assert a specific byte layout (untested territory - didn't want to
 * guess this backend's exact alignment rules and risk a spurious FAIL
 * from a wrong assumption rather than a real bug) - it checks the
 * array stride agrees with sizeof() and that adjacent elements don't
 * alias, which catches a padding/stride bug regardless of what the
 * "correct" layout number actually is. */
typedef struct {
	char tag;
	int  value;
	char flag;
} Item;

void set_flag(Item *p, char f)
{
	p->flag = f;
}

/* pass-by-value: must receive a copy - mutating it here must not be
 * visible to the caller */
void mutate_by_value(Item it)
{
	it.value = 999;
	it.flag = 'Z';
}

/* return-by-value: per cgen.c, any struct-typed result (typecmplx)
 * routes through sugen(), and swt.c's align() has an explicit
 * Aarg0 "passbyptr" case for struct arguments - both point at a
 * hidden-pointer convention, so this should be supported, not just
 * struct-by-pointer/by-value arguments */
Item make_item(char tag, int value, char flag)
{
	Item r;

	r.tag = tag;
	r.value = value;
	r.flag = flag;
	return r;
}

int main(void)
{
	Item arr[3];
	Item b, m;
	int stride1, stride2;

	gpio_led_init();
	uart_puts("test-struct starting\n");

	failed = 0;

	arr[0].tag = 'A'; arr[0].value = 100; arr[0].flag = 'x';
	arr[1].tag = 'B'; arr[1].value = 200; arr[1].flag = 'y';
	arr[2].tag = 'C'; arr[2].value = 300; arr[2].flag = 'z';

	check("arr[0].tag",   arr[0].tag,   'A');
	check("arr[0].value", arr[0].value, 100);
	check("arr[0].flag",  arr[0].flag,  'x');
	check("arr[1].value", arr[1].value, 200);
	check("arr[2].value", arr[2].value, 300);
	/* re-read arr[0] after writing arr[1]/arr[2] - a wrong stride
	 * would alias elements and corrupt this */
	check("arr[0].value unaffected by later writes", arr[0].value, 100);

	stride1 = (int)((char *)&arr[1] - (char *)&arr[0]);
	stride2 = (int)((char *)&arr[2] - (char *)&arr[1]);
	check("array stride == sizeof(Item)", stride1, (int)sizeof(Item));
	check("stride consistent between elements", stride2, stride1);

	b = arr[1];	/* struct assignment / copy */
	check("copy b.tag",   b.tag,   'B');
	check("copy b.value", b.value, 200);
	check("copy b.flag",  b.flag,  'y');
	b.value = 555;
	check("mutating copy doesn't affect original", arr[1].value, 200);

	set_flag(&arr[0], 'Q');
	check("set_flag via pointer", arr[0].flag, 'Q');

	mutate_by_value(arr[2]);
	check("pass-by-value does not mutate caller", arr[2].value, 300);

	m = make_item('M', 777, 'F');
	check("returned struct tag",   m.tag,   'M');
	check("returned struct value", m.value, 777);
	check("returned struct flag",  m.flag,  'F');

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
