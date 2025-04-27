    .section .text
    .globl _start

_start:
    /* Optional: Set stack pointer */
    la sp, stack_top

    /* Set PD18 to output mode */
    li t0, 0x02000098       /* PD_CFG2 register */
    li t1, 0x0FFFF1FF       /* Clear bits 19:18 */
    sw t1, 0(t0)            /* Write to PD_CFG2 */

    /* Small delay */
    li t2, 100000
delay1:
    addi t2, t2, -1
    bnez t2, delay1

    /* Set PD18 high */
    li t0, 0x020000A0       /* PD_DATA register */
    li t1, 0x00040000       /* Bit 18 = 1 */
    sw t1, 0(t0)            /* Turn LED on */

loop:
    j loop                 /* Loop forever */

    /* Reserve a small stack space */
    .section .bss
    .align 4
    .global stack_top
stack:
    .space 4096
stack_top:

