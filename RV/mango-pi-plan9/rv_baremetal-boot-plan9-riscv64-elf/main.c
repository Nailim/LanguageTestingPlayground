
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Deliberately trivial - the point of this folder is validating the
 * LINKER's ELF-emission path (il/jl's elf.c/elf.h, -H 5), not testing
 * any more C-language surface. Everything here is already proven by
 * earlier folders in this project.
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

int main(void)
{
	uart_puts("Hello from an ELF-built Plan9 riscv64 image\n");
	while(1)
		dummy(0);
	return(0);
}
