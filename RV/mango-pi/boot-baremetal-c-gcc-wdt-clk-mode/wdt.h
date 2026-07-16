#ifndef WDT_H
#define WDT_H

/*
 * D1 has (at least) two independent watchdog instances - see wdt.c for
 * the register details. Each has its own enable/disable/feed so you can
 * comment out any combination in main.c to see which one (if either) is
 * actually armed by default and responsible for a reset.
 */

void wdt_main_enable(unsigned int timeout_seconds);
void wdt_main_disable(void);
void wdt_main_feed(void);

void wdt_riscv_enable(unsigned int timeout_seconds);
void wdt_riscv_disable(void);
void wdt_riscv_feed(void);

#endif
