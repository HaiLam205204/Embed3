#include "gpio.h"

// 10.2
struct timer_regs
{
    unsigned int control_status;
    unsigned int counter_lo;
    unsigned int counter_hi;
    unsigned int compare[4];
};

#define REGS_TIMER ((volatile struct timer_regs *)(unsigned long)(MMIO_BASE + 0x00003000))

void timer_init();
unsigned long timer_get_ticks();