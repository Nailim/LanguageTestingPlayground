
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIO_BASE 0x02000000
#define PD_CFG2 (GPIO_BASE+0x0098)
#define PD_DATA (GPIO_BASE+0x00A0)
#define PD_LED  0x00040000

/*
 * Real linked calls, not inlined - the point of this folder is to confirm
 * objtype=riscv64 generates working CALL/RET (argument passing and return
 * values), not just the plain JMP that rv_baremetal-boot-plan9-riscv64's
 * _start->main transfer already validated on hardware.
 *
 * PUT32 exercises a call with two arguments and no return value used.
 * dummy exercises a call with one argument in a spin loop.
 * GET32 exercises a call with a return value that the caller actually
 * consumes: the next PUT32 write is computed from GET32's result, so a
 * wrong return-register convention shows up as a stuck or erratic LED
 * instead of a clean toggle, rather than being silently optimized away.
 */
int main ( void )
{
    unsigned int rx;
    unsigned int val;

    PUT32(PD_CFG2, 0x0FFFF1FF);
    PUT32(PD_DATA, PD_LED);

    while(1)
    {
        for(rx=0;rx<0x1000000;rx++) dummy(rx);
        val = GET32(PD_DATA);
        PUT32(PD_DATA, val ^ PD_LED);
    }
    return(0);
}
