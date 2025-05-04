#include "game.h"
#include "framebf.h"
#include "../uart/uart0.h"
#include "gpio.h"
#include "utils.h"
#include "renderFrame.h"
#include "game_map.h"
#include "protagonist_sprite.h"

#define GAME_FRAME_RATE 10             // e.g., 10 FPS
#define GAME_FRAME_US (1000000/GAME_FRAME_RATE) // microseconds per frame

#define NULL ((void *)0)

int map_x = MAP_START_X;
int map_y = MAP_START_Y;

// You can adjust this buffer size to suit your maximum sprite dimensions
static unsigned long sprite_bg_buffer[PROTAG_WIDTH * PROTAG_HEIGHT];

void game_loop() {
    
    int protag_x = PROTAG_START_X;
    int protag_y = PROTAG_START_Y;
    int prev_protag_x = protag_x;
    int prev_protag_y = protag_y;

    int first_frame = 1;

    while (1) {
        uint64_t start_time = get_arm_system_time();

        if (first_frame) {
            // Full draw on first frame
            for (int i = 0; i < 3; i++) {
            // Clear the back buffer
            clear_screen(0xFFFF0000); // Bright red
            drawImage_double_buffering(map_x, map_y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
            drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
            __asm volatile("dmb ish" ::: "memory");
            swap_buffers();
            wait_us(16000);         // ~1 frame at 60Hz in microseconds
            }
            first_frame = !first_frame;
        } else {
            // Only update changed areas 

            // 1. Handle input
            // if (button_pressed(UP)) player_y--;
            // if (button_pressed(DOWN)) player_y++;
            // // ... other directions

            // 1. Handle input and update game state
            // Update previous position
            prev_protag_x = protag_x;
            prev_protag_y = protag_y;
            //update_protag_position(&protag_x, &protag_y)
            if (prev_protag_x != protag_x || prev_protag_y != protag_y) {
                // Restore background where protagonist *was*
                draw_partial_map(prev_protag_x, prev_protag_y);

                // Draw the sprite at the new position
                drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
            }
            __asm volatile("dmb ish" ::: "memory");
            // Swap buffers to display the new frame
            swap_buffers();
        }

        // Frame timing
        uint64_t end_time = get_arm_system_time();

        uint64_t render_time = end_time - start_time;

        uint64_t render_time_us = ticks_to_us(render_time);

        if (render_time_us < GAME_FRAME_US) {
            uart_puts("Waiting for frame interval");
            wait_us(GAME_FRAME_US - render_time_us);
        }
    }   
}

void draw_partial_map(int x, int y) {
    // Calculate local coordinates relative to the map
    int local_x = x - map_x;
    int local_y = y - map_y;

    // Extract the part of the map that was covered by the sprite
    unsigned long* map_section = extract_subimage_static(game_map, map_x, map_y, local_x, local_y, PROTAG_WIDTH, PROTAG_HEIGHT, sprite_bg_buffer);

    // Only draw if extraction was successful (i.e., bounds were valid)
    if (map_section) {
        drawImage_double_buffering(x, y, map_section, PROTAG_WIDTH, PROTAG_HEIGHT);
        uart_puts("Redraw successfully!");
    } else {
        uart_puts("Player out of bounds!");
    }
}


/**
 * Extracts a rectangular portion of an image (subimage) from a larger image buffer.
 * 
 * src         Pointer to the full source image buffer (1D array, row-major layout).
 * src_width   Width of the source image in pixels.
 * src_height  Height of the source image in pixels.
 * start_x     X coordinate (column) of the top-left corner of the subimage within the source image.
 * start_y     Y coordinate (row) of the top-left corner of the subimage within the source image.
 * width       Width of the subimage to extract.
 * height      Height of the subimage to extract.
 * out_buffer  Pointer to a pre-allocated buffer where the extracted subimage will be stored.
 *                    This buffer must be at least width * height * sizeof(uint32_t) in size.
 */
unsigned long* extract_subimage_static(const unsigned long* src, int src_width, int src_height, int start_x, int start_y, int width, int height, unsigned long* out_buffer) {
    
    // Bounds check to ensure subimage fits within the source image
    if (start_x < 0 || start_y < 0 ||
        start_x + width > src_width || start_y + height > src_height) {
        // Invalid region — do nothing (or handle error as needed)
        return NULL;
    }

    // Loop over each row of the subimage
    for (int y = 0; y < height; ++y) {
        // Compute the starting address of the row in the source image
        // Formula: (start_y + y) * src_width → gives the starting index of the row in the full image
        // + start_x → shifts to the desired subimage column
        const unsigned long* src_row = src + (start_y + y) * src_width + start_x;

        // Compute the destination row in the output buffer
        // Since out_buffer is a flat array, each row starts at y * width
        unsigned long* dst_row = out_buffer + y * width;

        // Copy one row (width pixels) from the source image to the output buffer
        // This uses memcpy to copy width * 4 bytes (assuming 32-bit pixels)
        memcpy(dst_row, src_row, width * sizeof(uint32_t));

        return out_buffer;
    }
}