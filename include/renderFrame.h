#include "gpio.h"

#define FRAME_RATE 30                 // e.g., 30 FPS
#define FRAME_US (1000000/FRAME_RATE) // microseconds per frame

void draw_background();
uint64_t get_arm_timer_freq();
uint64_t get_arm_system_time();
uint64_t ticks_to_us(uint64_t ticks);