#include "gpio.h"

#define CLOCKHZ 1000000

// RPI4 p.143
#define SYSTEM_TIMER_CS (*(volatile unsigned int *)(MMIO_BASE + 0x00003000)) // System Timer Control/Status
#define SYSTEM_TIMER_CLO (*(volatile unsigned int *)(MMIO_BASE + 0x00003004)) // System Timer Counter Lower 32 bits
#define SYSTEM_TIMER_CHI (*(volatile unsigned int *)(MMIO_BASE + 0x00003008)) // System Timer Counter Higher 32 bits
#define SYSTEM_TIMER_C1 (*(volatile unsigned int *)(MMIO_BASE + 0x00003010))  // System Timer Compare 1
#define SYSTEM_TIMER_C3 (*(volatile unsigned int *)(MMIO_BASE + 0x00003018))  // System Timer Compare 3

// RPI4 p.87 - 88
#define SYS_TIMER_IRQ_1 2 // 0 0 1 0 TIMER 1 is in number 1 in the VC peripherals
#define SYS_TIMER_IRQ_3 8 // 1 0 0 0 TIMER 3 is in number 3 in the VC peripherals

void timer_init();
void handle_timer_1();
void handle_timer_3();
void timer_sleep(uint32_t ms);
uint64_t timer_get_ticks();
