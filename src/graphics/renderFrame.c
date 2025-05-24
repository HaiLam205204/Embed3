#include "../../include/renderFrame.h"
#include "../../include/timer.h"
#include "../../include/framebf.h"
#include "../../include/utils.h"
#include "../../include/uart0.h"
#include "../../include/bitmaps/welcomeScreen.h"

void draw_background() {
    drawImage(0, 0, background, 1024, 768);
    drawString(0, 0, "Nguyen Hai Lam", 0x0000ACFF, 7);
    drawString(0, 200, "Nguyen Trong Khoa", 0x0000AE00, 7);
    drawString(0, 400, "Nguyen Duc Anh", 0x00CC00FF, 7);
    drawString(0, 600, "Ung Xuan Dat", 0x00FFAC00, 7);
}

// // GPU sync frame rendering
// // Use when hardware interaction (GPU, interrupts) are needed
// void render_frame() {
//     uint64_t start = get_system_timer(); // From 0x3F003000
//     draw_graphics();
//     uint64_t elapsed = get_system_timer() - start;
//     if (elapsed < 16667) delay_us(16667 - elapsed); // 60 FPS
// }

// Get the timer frequency (Hz)
uint64_t get_arm_timer_freq() {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

// Read the 64-bit system timer (return ticks from clock)
uint64_t get_arm_system_time() {
    uint64_t cnt;
    asm volatile("mrs %0, cntpct_el0" : "=r"(cnt));  // Read counter
    return cnt;
}

// Convert ticks to microseconds
// This function avoid overflow issue
uint64_t ticks_to_us(uint64_t ticks) {
    uint64_t freq = get_arm_timer_freq();
    return (ticks / freq) * 1000000ULL // Full seconds in microseconds
            + ((ticks % freq) * 1000000ULL) / freq; // Remeainder in microseconds
}