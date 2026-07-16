#ifndef MMIO_H
#define MMIO_H

static inline void reg_write32(unsigned long addr, unsigned int val)
{
	*(volatile unsigned int *)addr = val;
}

static inline unsigned int reg_read32(unsigned long addr)
{
	return *(volatile unsigned int *)addr;
}

#endif
