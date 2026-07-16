
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIO_BASE 0x02000000
#define PD_CFG2 (GPIO_BASE+0x0098)
#define PD_DATA (GPIO_BASE+0x00A0)

/*
 * Same shape as rv_baremetal-boot-plan9's original (pre-workaround)
 * main.c - real calls to PUT32/GET32/dummy, not inlined. The point of
 * this folder is to check whether riscv64 (objtype=riscv64) generates
 * calls that actually work on the D1's RV64 hart, unlike objtype=riscv
 * (RV32), whose C.JAL calls silently misdecode as C.ADDIW on RV64
 * hardware - see MangoBoot memory: plan9-riscv-cjal-collision.
 */
int main ( void )
{
    unsigned int rx;

    PUT32(PD_CFG2, 0x0FFFF1FF);

    while(1)
    {
        PUT32(PD_DATA, 0x00040000);
        for(rx=0;rx<0x1000000;rx++) dummy(rx);
        PUT32(PD_DATA, 0x00000000);
        for(rx=0;rx<0x1000000;rx++) dummy(rx);
    }
    return(0);
}
