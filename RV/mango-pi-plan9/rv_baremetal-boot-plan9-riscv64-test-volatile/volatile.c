/*
 * The code under test: every memory access below goes through a
 * `volatile`-qualified pointer directly - no PUT32/GET32 helper calls
 * anywhere in this file. See README for what each test targets and why.
 */

typedef unsigned int u32int;

#define GPIO_BASE 0x02000000
#define PD_CFG2   (GPIO_BASE + 0x0098)
#define PD_DATA   (GPIO_BASE + 0x00A0)
#define LED_PIN        18
#define LED_BIT        (1 << LED_PIN)
#define LED_CFG_SHIFT  ((LED_PIN - 16) * 4)
#define LED_CFG_MASK   (0xF << LED_CFG_SHIFT)
#define LED_CFG_OUTPUT (0x1 << LED_CFG_SHIFT)

/* --- dead-store elimination: does a volatile store survive even though
 * nothing in the same function reads it back? --- */
volatile u32int storetarget;

void
vol_store(u32int v)
{
	volatile u32int *p = &storetarget;
	*p = v;
}

u32int
vol_load(void)
{
	volatile u32int *p = &storetarget;
	return *p;
}

/* --- no caching/hoisting of a volatile read across a loop --- */
volatile int pollflag;

int
vol_poll(void)
{
	int i;

	pollflag = 0;
	for(i = 0; i < 1000000; i++){
		if(i == 37)
			pollflag = 1;
		if(pollflag)
			return i;
	}
	return -1;
}

/* --- real hardware: the on-board LED, driven purely through volatile
 * pointers, no PUT32/GET32 anywhere in this path --- */
void
vol_led_init(void)
{
	volatile u32int *cfg = (volatile u32int*)PD_CFG2;
	u32int v;

	v = *cfg;
	v &= ~LED_CFG_MASK;
	v |= LED_CFG_OUTPUT;
	*cfg = v;
}

void
vol_led_set(int on)
{
	volatile u32int *data = (volatile u32int*)PD_DATA;
	u32int v;

	v = *data;
	if(on)
		v |= LED_BIT;
	else
		v &= ~LED_BIT;
	*data = v;
}

int
vol_led_read(void)
{
	volatile u32int *data = (volatile u32int*)PD_DATA;
	return (*data & LED_BIT) != 0;
}
