
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIO_BASE 0x02000000
#define PD_CFG2 (GPIO_BASE+0x0098)
#define PC_DATA (GPIO_BASE+0x00A0)

int main ( void )
{
    unsigned int rx;

    // Configure PD18 as output
    rx=GET32(PD_CFG2);
    rx&=~(0x00000F00);
    rx|= (0x00000100);
    PUT32(PD_CFG2,rx);

    while(1)
    {
        PUT32(PC_DATA, 0x00040000);
        for(rx=0;rx<0x1000000;rx++) dummy(rx);
        PUT32(PC_DATA, 0x00000000);
        for(rx=0;rx<0x1000000;rx++) dummy(rx);
    }
    return(0);
}
