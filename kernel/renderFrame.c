#include "renderFrame.h"
#include "timer.h"
#include "framebf.h"
#include "utils.h"

void video_playback(const unsigned long** frames, uint32_t frame_count, int x, int y, int src_width, int src_height, int max_width, int max_height) {
    uint32_t current_frame = 0;
    
    // Initialize timer for first frame
    set_wait_timer(1, FRAME_US);
    
    while (current_frame < frame_count) {
        // 1. Display current frame (implement your display function)
        drawImageScaledAspect(x, y, frames[current_frame], src_width, src_height, max_width, max_height);
        // 2. Wait for next frame time
        set_wait_timer(0, 0); // Uses previously set expiration time
        
        // 3. Prepare timer for next frame
        set_wait_timer(1, FRAME_US);
        
        // 4. Advance to next frame (with optional loop handling)
        current_frame++;
        if (current_frame >= frame_count) {
            current_frame = 0; // Loop video if desired
        }
    }
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