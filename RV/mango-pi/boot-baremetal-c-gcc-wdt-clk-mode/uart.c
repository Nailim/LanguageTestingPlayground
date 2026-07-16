#include "uart.h"
#include "mmio.h"

/*
 * D1 UART0, a Synopsys DesignWare 16550-compatible UART (see
 * arch/riscv/dts/sun20i-d1.dtsi: uart0 @ 0x2500000, reg-io-width=4,
 * reg-shift=2 -> each register is a 32-bit word, 4 bytes apart).
 *
 * U-Boot already configures clocks, pinmux and baud rate for this UART
 * as its own console before handing off to us, so we only need THR/LSR.
 */
#define UART0_BASE 0x02500000UL

#define UART_THR (UART0_BASE + (0 * 4)) /* transmit holding register */
#define UART_LSR (UART0_BASE + (5 * 4)) /* line status register */

#define UART_LSR_THRE (1 << 5) /* transmit holding register empty */

void uart_putc(char c)
{
	while (!(reg_read32(UART_LSR) & UART_LSR_THRE))
		;
	reg_write32(UART_THR, (unsigned char)c);
}

void uart_puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			uart_putc('\r');
		uart_putc(*s++);
	}
}
