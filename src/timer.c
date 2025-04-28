#include "timer.h"
#include "uart.h"
#include "video.h"


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

const uint32_t interval_1 = CLOCKHZ / 30;
uint32_t cur_val_1 = 0;

const uint32_t interval_3 = CLOCKHZ / 30;
uint32_t cur_val_3 = 0;

void timer_init() {
    cur_val_1 = SYSTEM_TIMER_CLO;
    cur_val_1 += interval_1;
    SYSTEM_TIMER_C1 = cur_val_1;

    cur_val_3 = SYSTEM_TIMER_CLO;
    cur_val_3 += interval_3;
    SYSTEM_TIMER_C3 = cur_val_3;
}

extern int current_frame;

extern int state;
extern int is_welcome_screen;

// Current position of the top-left corner of the video
int video_x = (SCREEN_WIDTH - VIDEO_WIDTH) / 2;
int video_y = (SCREEN_HEIGHT - VIDEO_HEIGHT) / 2;

// Current position of the top-left corner of the welcome screen
// int welcome_x = (SCREEN_WIDTH - WELCOME_WIDTH) / 2;
// int welcome_y = (SCREEN_HEIGHT - WELCOME_HEIGHT) / 2;

int current_frame = 0; // Keep track of current frame

void handle_timer_1() {
    if (state == 1) {
        // Move to the next frame
        current_frame = (current_frame + 1) % epd_bitmap_allArray_LEN;
        // Draw the current frame of the video
        drawImage(video_x, video_y, epd_bitmap_allArray[current_frame], VIDEO_WIDTH, VIDEO_HEIGHT);
    } 

    cur_val_1 += interval_1;
    SYSTEM_TIMER_C1 = cur_val_1;
    SYSTEM_TIMER_CS |= SYS_TIMER_IRQ_1;
}

void handle_timer_3() {
    cur_val_3 += interval_3;
    SYSTEM_TIMER_C3 = cur_val_3;
    SYSTEM_TIMER_CS |= SYS_TIMER_IRQ_3;
}
