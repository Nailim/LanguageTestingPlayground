
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Compiler test: does objtype=riscv64's `ic` honor `volatile` well enough
 * to retire the PUT32/GET32 asm-helper MMIO pattern established back in
 * Phase 0/1? See README for what this test targets and why.
 *
 * Reporting stays on the proven PUT32/GET32 UART path deliberately - if
 * volatile turns out to be broken, we still want a working way to say so.
 * The GPIO/LED functions under test (vol_led_*) are the only thing in
 * this file that touch real hardware through volatile pointers instead.
 */

#define UART0_BASE 0x02500000
#define UART_THR (UART0_BASE + (0 * 4))
#define UART_LSR (UART0_BASE + (5 * 4))
#define UART_LSR_THRE (1 << 5)

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

/* volatile.c - the code under test */
void vol_store(unsigned int);
unsigned int vol_load(void);
int vol_poll(void);
void vol_led_init(void);
void vol_led_set(int);
int vol_led_read(void);

int main(void)
{
	vol_led_init();
	uart_puts("test-volatile starting\n");

	failed = 0;

	uart_puts("-- dead-store elimination --\n");
	vol_store(0xcafebabe);
	check("vol_store/vol_load roundtrip", vol_load(), 0xcafebabe);

	uart_puts("-- no read caching/hoisting across a loop --\n");
	check("vol_poll (exits right at iteration 37)", vol_poll(), 37);

	uart_puts("-- real GPIO via volatile pointers only --\n");
	vol_led_set(1);
	check("vol_led_read after set(1)", vol_led_read(), 1);
	vol_led_set(0);
	check("vol_led_read after set(0)", vol_led_read(), 0);

	if(failed == 0)
		uart_puts("ALL TESTS PASSED\n");
	else
		uart_puts("SOME TESTS FAILED (see above)\n");

	/* heartbeat: solid LED = done, all passed. Fast blink = done, at
	 * least one FAIL was printed above. Deliberately reuses vol_led_set
	 * (not a PUT32-based helper) - if it's broken, the blink itself
	 * fails to appear too, which is informative on its own. */
	while(1){
		if(failed == 0){
			vol_led_set(1);
		}else{
			vol_led_set(1);
			delay(2000000);
			vol_led_set(0);
			delay(2000000);
		}
	}
	return(0);
}
