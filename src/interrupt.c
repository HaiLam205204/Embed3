#include "interrupt.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"

void enable_interrupt_controller() {
#if RPI_VERSION // RPI3
    IRQ_ENABLE_1 = AUX_IRQ | SYS_TIMER_IRQ_1 | SYS_TIMER_IRQ_3;  // Enable both interrupt and timer
#else // RPI4
    IRQ0_SET_EN_0 = AUX_IRQ | SYS_TIMER_IRQ_1 | SYS_TIMER_IRQ_3; // Enable both interrupt and timer
#endif
}

void handle_irq() {
    uint32_t irq;

#if RPI_VERSION // RPI3
    irq = IRQ_PENDING_1;
#else // RPI4
    irq = IRQ0_PENDING0;
#endif

    if (irq & AUX_IRQ) {
        irq &= ~AUX_IRQ;
    }

    if (irq & SYS_TIMER_IRQ_1) {
        irq &= ~SYS_TIMER_IRQ_1;
        handle_timer_1();
    }

    if (irq & SYS_TIMER_IRQ_3) {
        irq &= ~SYS_TIMER_IRQ_3;

        handle_timer_3();
    }
}
