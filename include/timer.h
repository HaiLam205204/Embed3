#include "gpio.h"

#define SYSTEM_TIMER_OFFSET 0x3000
#define SYSTEM_TIMER_BASE (MMIO_BASE + SYSTEM_TIMER_OFFSET)

// define system timer GPIO based on their address

#define SYSTEM_TIMER_CS (* (volatile unsigned int*)(TIMER_BASE+0x0))
#define SYSTEM_TIMER_CLO (* (volatile unsigned int*)(TIMER_BASE+0x4))
#define SYSTEM_TIMER_CHI (* (volatile unsigned int*)(TIMER_BASE+0x8))
#define SYSTEM_TIMER_C0 (* (volatile unsigned int*)(TIMER_BASE+0xc))
#define SYSTEM_TIMER_C1 (* (volatile unsigned int*)(TIMER_BASE+0x10))
#define SYSTEM_TIMER_C2 (* (volatile unsigned int*)(TIMER_BASE+0x14))
#define SYSTEM_TIMER_C3 (* (volatile unsigned int*)(TIMER_BASE+0x18))


