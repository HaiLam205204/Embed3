#include "timer.h"

// Timer and counter to get current ticks
void timer_init() {
    unsigned int cur_val_1 = 0;
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += 1000000;
    REGS_TIMER->compare[1] = cur_val_1;
}

unsigned long timer_get_ticks() {
    unsigned int hi = REGS_TIMER->counter_hi;
    unsigned int lo = REGS_TIMER->counter_lo;

    //double check hi value didn't change after setting it...
    if (hi != REGS_TIMER->counter_hi) {
        hi = REGS_TIMER->counter_hi;
        lo = REGS_TIMER->counter_lo;
    }

    return ((unsigned long)hi << 32) | lo;
}