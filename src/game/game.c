#include "../../include/game.h"
#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/gpio.h"
#include "../../include/utils.h"
#include "../../include/renderFrame.h"
#include "../../include/game_map.h"
#include "../../include/protagonist_sprite.h"

#define GAME_FRAME_RATE 30                        // e.g., 30 FPS
#define GAME_FRAME_US (1000000 / GAME_FRAME_RATE) // microseconds per frame

#define PIXEL_SIZE 4 // Assuming 32-bit pixels (4 bytes per pixel)
#define NULL ((void *)0)

#define RESTORE_MARGIN 10
// The safety margin (RESTORE_MARGIN) ensures complete coverage of the changed pixels
static unsigned long sprite_bg_buffer[(PROTAG_WIDTH + 2 * RESTORE_MARGIN) * (PROTAG_HEIGHT + 2 * RESTORE_MARGIN)];

// MIN, MAX
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Map starting coordinates
int map_x = MAP_START_X;
int map_y = MAP_START_Y;

// Global game state
int protag_world_x = PROTAG_START_X;  // World coordinates
int protag_world_y = PROTAG_START_Y;
int map_offset_x = 0;                 // Current map offset
int map_offset_y = 0;

void update_protag_position(int *world_x, int *world_y, char direction) {
    const int step_size = RESTORE_MARGIN;
    
    switch (direction) {
        case UP:    *world_y -= step_size; break;
        case DOWN:  *world_y += step_size; break;
        case LEFT:  *world_x -= step_size; break;
        case RIGHT: *world_x += step_size; break;
        default: break;
    }
    
    // Debug output can remain as in your original
}

void game_loop() {
    int first_frame = 1;
    char input;
    int screen_center_x = SCREEN_WIDTH / 2;
    int screen_center_y = SCREEN_HEIGHT / 2;

    uart_puts("\n[GAME_LOOP] Starting game loop with virtual offset scrolling");

    while (1) {
        uint64_t start_time = get_arm_system_time();
        
        if (first_frame) {
            // Initial full render
            for (int i = 0; i < 2; i++) {
                clear_screen(0x00000000);
                drawImage_double_buffering(-map_offset_x, -map_offset_y, 
                                        game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
                drawImage_double_buffering(screen_center_x, screen_center_y, 
                                        myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
                swap_buffers();
                wait_us(16000);
            }
            first_frame = 0;
        }

        // Handle input
        input = uart_getc();
        
        // Store previous world position
        int prev_world_x = protag_world_x;
        int prev_world_y = protag_world_y;
        
        // Update position in world coordinates
        update_protag_position(&protag_world_x, &protag_world_y, input);

        // Calculate required map offset to keep protagonist centered
        int new_offset_x = protag_world_x - screen_center_x;
        int new_offset_y = protag_world_y - screen_center_y;
        
        // Clamp offsets to map boundaries
        new_offset_x = MAX(0, MIN(new_offset_x, GAME_MAP_WIDTH - SCREEN_WIDTH));
        new_offset_y = MAX(0, MIN(new_offset_y, GAME_MAP_HEIGHT - SCREEN_HEIGHT));

        // Only redraw if something changed
        if (new_offset_x != map_offset_x || new_offset_y != map_offset_y) {
            map_offset_x = new_offset_x;
            map_offset_y = new_offset_y;
            
            // Update virtual offset through mailbox
            set_virtual_offset(map_offset_x, map_offset_y);
            
            // Full redraw (simplest approach)
            clear_screen(0x00000000);
            drawImage_double_buffering(-map_offset_x, -map_offset_y, 
                                     game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
            drawImage_double_buffering(screen_center_x, screen_center_y, 
                                     myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
        } else if (prev_world_x != protag_world_x || prev_world_y != protag_world_y) {
            // Only protagonist moved - just redraw protagonist
            draw_partial_map(screen_center_x, screen_center_y);  // Restore old position
            drawImage_double_buffering(screen_center_x, screen_center_y, 
                                     myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
        }

        swap_buffers();

        // Frame timing control
        uint64_t end_time = get_arm_system_time();
        uint64_t render_time_us = ticks_to_us(end_time - start_time);
        
        if (render_time_us < GAME_FRAME_US) {
            wait_us(GAME_FRAME_US - render_time_us);
        } else {
            uart_puts("\n[WARNING] Frame took too long!");
        }
    }
}

// void game_loop()
// {
//     // Game state variables
//     int protag_x = PROTAG_START_X;
//     int protag_y = PROTAG_START_Y;
//     int prev_protag_x = protag_x;
//     int prev_protag_y = protag_y;
//     int first_frame = 1;
//     char input;

//     uart_puts("\n[GAME_LOOP] Starting game loop");
//     uart_puts("\n[GAME_LOOP] Initial position: (");
//     uart_dec(protag_x);
//     uart_puts(",");
//     uart_dec(protag_y);
//     uart_puts(")");

//     while (1)
//     {
//         uint64_t start_time = get_arm_system_time();
//         uart_puts("\n[FRAME] ---- NEW FRAME ----");

//         if (first_frame)
//         { // display first frame
//             uart_puts("\n[FRAME] Rendering first frame");
//             for (int i = 0; i < 2; i++)
//             {
//                 clear_screen(0xFFFF0000); // bright red
//                 uart_puts("\n[FRAME] Cleared screen (red)");

//                 // draw game map
//                 drawImage_double_buffering(map_x, map_y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
//                 uart_puts("\n[FRAME] Drawn map at (");
//                 uart_dec(map_x);
//                 uart_puts(",");
//                 uart_dec(map_y);
//                 uart_puts(")");

//                 // draw protagonist
//                 drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
//                 uart_puts("\n[FRAME] Drawn protagonist at (");
//                 uart_dec(protag_x);
//                 uart_puts(",");
//                 uart_dec(protag_y);
//                 uart_puts(")");

//                 // swap buffer to display frame
//                 swap_buffers();
//                 uart_puts("\n[FRAME] Swapped buffers");
//                 wait_us(16000);
//             }
//             first_frame = !first_frame; // toggle flag
//         }
//         else
//         { // display second frame onwards

//             // get input from user
//             input = uart_getc();
//             uart_puts("\n[INPUT] Received: ");
//             uart_sendc(input); // Echo the input character

//             // update protagonist position based on input
//             update_protag_position(&protag_x, &protag_y, input);
//             uart_puts("\n[POSITION] New position: (");
//             uart_dec(protag_x);
//             uart_puts(",");
//             uart_dec(protag_y);
//             uart_puts(")");

//             // check new position compared to old one
//             if (prev_protag_x != protag_x || prev_protag_y != protag_y)
//             {
//                 uart_puts("\n[MOVEMENT] Position changed - redrawing");
//                 // Restore background where sprite WAS (with margins)
//                 draw_partial_map(prev_protag_x, prev_protag_y); // replace the old position of protagonist with the map

//                 // Draw new sprite position
//                 drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
//                 uart_puts("\n[MOVEMENT] Redrew protagonist at new position");

//                 prev_protag_x = protag_x;
//                 prev_protag_y = protag_y;
//             }
//             swap_buffers();
//         }

//         // keep track rendering time
//         uint64_t end_time = get_arm_system_time();
//         uint64_t render_time_us = ticks_to_us(end_time - start_time);
//         uart_puts("\n[TIMING] Frame render time (us): ");
//         uart_dec(render_time_us);

//         if (render_time_us < GAME_FRAME_US)
//         {
//             uint64_t wait_time = GAME_FRAME_US - render_time_us;
//             uart_puts("\n[TIMING] Waiting (us): ");
//             uart_dec(wait_time);
//             wait_us(wait_time);
//         }
//         else
//         {
//             uart_puts("\n[WARNING] Frame took too long!");
//         }
//     }
// }

// // draw sprite at a new location
// void update_protag_position(int *x, int *y, char direction)
// {
//     const int step_size = RESTORE_MARGIN;
//     int old_x = *x;
//     int old_y = *y;

//     uart_puts("\n[UPDATE_POS] Direction: ");
//     uart_sendc(direction);
//     uart_puts(" Old position: (");
//     uart_dec(old_x);
//     uart_puts(",");
//     uart_dec(old_y);
//     uart_puts(")");

//     switch (direction)
//     {
//     case UP:
//         *y -= step_size;
//         break;
//     case DOWN:
//         *y += step_size;
//         break;
//     case LEFT:
//         *x -= step_size;
//         break;
//     case RIGHT:
//         *x += step_size;
//         break;
//     default:
//         uart_puts("\n[UPDATE_POS] Invalid direction!");
//         break;
//     }

//     uart_puts(" New position: (");
//     uart_dec(*x);
//     uart_puts(",");
//     uart_dec(*y);
//     uart_puts(")");
// }

// draw a fraction of the map
void draw_partial_map(int prev_x, int prev_y)
{
    // Convert screen coords to map coords with safety margins
    int map_local_x = prev_x - map_x - RESTORE_MARGIN;
    int map_local_y = prev_y - map_y - RESTORE_MARGIN;

    // Calculate dimensions with margins
    int restore_width = PROTAG_WIDTH + 2 * RESTORE_MARGIN;
    int restore_height = PROTAG_HEIGHT + 2 * RESTORE_MARGIN;

    // Clamp to map boundaries
    if (map_local_x < 0)
    {
        restore_width += map_local_x; // Reduces width
        map_local_x = 0;
    }
    if (map_local_y < 0)
    {
        restore_height += map_local_y;
        map_local_y = 0;
    }
    if (map_local_x + restore_width > GAME_MAP_WIDTH)
    {
        restore_width = GAME_MAP_WIDTH - map_local_x;
    }
    if (map_local_y + restore_height > GAME_MAP_HEIGHT)
    {
        restore_height = GAME_MAP_HEIGHT - map_local_y;
    }

    // Debug output
    uart_puts("\n[RESTORE] Prev(");
    uart_dec(prev_x);
    uart_puts(",");
    uart_dec(prev_y);
    uart_puts(") MapLocal(");
    uart_dec(map_local_x);
    uart_puts(",");
    uart_dec(map_local_y);
    uart_puts(") Size:");
    uart_dec(restore_width);
    uart_puts("x");
    uart_dec(restore_height);

    // Extract and restore
    unsigned long *bg_section = extract_subimage_static(
        game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT,
        map_local_x, map_local_y,
        restore_width, restore_height,
        sprite_bg_buffer);

    if (bg_section)
    {
        // Draw at adjusted screen position
        int screen_x = map_local_x + map_x;
        int screen_y = map_local_y + map_y;
        drawImage_double_buffering(screen_x, screen_y, bg_section, restore_width, restore_height);
    }
}

// extract the part of the map that was hidden by the sprite
unsigned long *extract_subimage_static(const unsigned long *src, int src_w, int src_h,
                                       int start_x, int start_y, int w, int h,
                                       unsigned long *buffer)
{
    // Strict alignment check
    if (((unsigned long)src | (unsigned long)buffer) & 0x3)
    {
        uart_puts("\n[ALIGN] Misaligned! src:");
        uart_hex((unsigned long)src);
        uart_puts(" buf:");
        uart_hex((unsigned long)buffer);
        return NULL;
    }

    // Validate bounds
    if (start_x < 0 || start_y < 0 || start_x + w > src_w || start_y + h > src_h)
    {
        uart_puts("\n[BOUNDS] Invalid: X[");
        uart_dec(start_x);
        uart_puts("..");
        uart_dec(start_x + w);
        uart_puts("] Y[");
        uart_dec(start_y);
        uart_puts("..");
        uart_dec(start_y + h);
        uart_puts("]");
        return NULL;
    }

    // Row-by-row copy with memcpy
    for (int y = 0; y < h; y++)
    {
        const unsigned long *src_row = src + (start_y + y) * src_w + start_x;
        unsigned long *dst_row = buffer + y * w;
        memcpy(dst_row, src_row, w * sizeof(unsigned long));
    }

    return buffer;
}