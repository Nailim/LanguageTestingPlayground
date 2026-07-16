
#define GPIO_BASE 0x02000000
#define PD_CFG2 (GPIO_BASE+0x0098)
#define PD_DATA (GPIO_BASE+0x00A0)

/*
 * GPIO access is inlined as direct pointer stores instead of going through
 * PUT32/GET32 calls (as start.s used to provide). Any `CALL`-with-link on
 * this objtype (riscv, i.e. RV32) compiles to C.JAL, which collides with
 * RV64C's C.ADDIW at the same encoding - the D1's C906 core runs in RV64
 * mode by the time this binary gets control, so C.JAL silently decodes as
 * C.ADDIW instead of a jump: the call never happens, no crash, no blink.
 * Until objtype=riscv64 is usable, avoiding calls entirely is what lets
 * this example actually run on real hardware.
 */

int main ( void )
{
    unsigned int rx;

    *(unsigned int *)PD_CFG2 = 0x0FFFF1FF;

    while(1)
    {
        *(unsigned int *)PD_DATA = 0x00040000;
        for(rx=0;rx<0x1000000;rx++) ;
        *(unsigned int *)PD_DATA = 0x00000000;
        for(rx=0;rx<0x1000000;rx++) ;
    }
    return(0);
}
