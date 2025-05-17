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

#define VIEWPORT_WIDTH  1024  // Your screen width
#define VIEWPORT_HEIGHT 768  // Your screen height
#define WORLD_WIDTH     (GAME_MAP_WIDTH * 2)  // Example: Map is twice as big as screen
#define WORLD_HEIGHT    (GAME_MAP_HEIGHT * 2) // Adjust as needed

#define RESTORE_MARGIN 10
// The safety margin (RESTORE_MARGIN) ensures complete coverage of the changed pixels
static unsigned long sprite_bg_buffer[(PROTAG_WIDTH + 2 * RESTORE_MARGIN) * (PROTAG_HEIGHT + 2 * RESTORE_MARGIN)];

// map starting coordinates
int map_x = MAP_START_X;
int map_y = MAP_START_Y;

// Camera offset
int camera_x = 0;
int camera_y = 0;

// World coordinates of sprite
int protag_world_x = PROTAG_START_X;  
int protag_world_y = PROTAG_START_Y;

void game_loop()
{
    int protag_x = PROTAG_START_X;
    int protag_y = PROTAG_START_Y;
    int prev_protag_x = protag_x;
    int prev_protag_y = protag_y;
    int first_frame = 1;
    char input;

    uart_puts("\n[GAME_LOOP] Starting game loop");
    uart_puts("\n[GAME_LOOP] Initial position: (");
    uart_dec(protag_x);
    uart_puts(",");
    uart_dec(protag_y);
    uart_puts(")");

    while (1)
    {
        uint64_t start_time = get_arm_system_time();
        uart_puts("\n[FRAME] ---- NEW FRAME ----");

        if (first_frame)
        { // display first frame
            uart_puts("\n[FRAME] Rendering first frame");
            for (int i = 0; i < 2; i++)
            {
                clear_screen(0xFFFF0000); // bright red
                uart_puts("\n[FRAME] Cleared screen (red)");

                // draw game map
                drawImage_double_buffering(map_x, map_y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
                uart_puts("\n[FRAME] Drawn map at (");
                uart_dec(map_x);
                uart_puts(",");
                uart_dec(map_y);
                uart_puts(")");

                // draw protagonist
                drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
                uart_puts("\n[FRAME] Drawn protagonist at (");
                uart_dec(protag_x);
                uart_puts(",");
                uart_dec(protag_y);
                uart_puts(")");

                // swap buffer to display frame
                swap_buffers();
                uart_puts("\n[FRAME] Swapped buffers");
                wait_us(16000);
            }
            first_frame = !first_frame; // toggle flag
        }
        else
        { // display second frame onwards

            // // get input from user
            // input = uart_getc();
            // uart_puts("\n[INPUT] Received: ");
            // uart_sendc(input); // Echo the input character

            // // update protagonist position based on input
            // update_protag_position(&protag_x, &protag_y, input);
            // uart_puts("\n[POSITION] New position: (");
            // uart_dec(protag_x);
            // uart_puts(",");
            // uart_dec(protag_y);
            // uart_puts(")");

            // // check new position compared to old one
            // if (prev_protag_x != protag_x || prev_protag_y != protag_y)
            // {
            //     uart_puts("\n[MOVEMENT] Position changed - redrawing");
            //     // Restore background where sprite WAS (with margins)
            //     draw_partial_map(prev_protag_x, prev_protag_y); // replace the old position of protagonist with the map

            //     // Draw new sprite position
            //     drawImage_double_buffering(protag_x, protag_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
            //     uart_puts("\n[MOVEMENT] Redrew protagonist at new position");

            //     prev_protag_x = protag_x;
            //     prev_protag_y = protag_y;
            // }
            // swap_buffers();
            // Get input and update world position
            input = uart_getc();
            update_protag_position(&protag_world_x, &protag_world_y, input);
            
            // Update camera position to follow protagonist
            update_camera_position(protag_world_x, protag_world_y, &camera_x, &camera_y);
            
            // Render the visible portion of the world
            render_world_view(camera_x, camera_y);
            
            // Render protagonist relative to camera
            int protag_screen_x = protag_world_x - camera_x;
            int protag_screen_y = protag_world_y - camera_y;
            drawImage_double_buffering(protag_screen_x, protag_screen_y, 
                                    myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
            
            swap_buffers();
        }

        // keep track rendering time
        uint64_t end_time = get_arm_system_time();
        uint64_t render_time_us = ticks_to_us(end_time - start_time);
        uart_puts("\n[TIMING] Frame render time (us): ");
        uart_dec(render_time_us);

        if (render_time_us < GAME_FRAME_US)
        {
            uint64_t wait_time = GAME_FRAME_US - render_time_us;
            uart_puts("\n[TIMING] Waiting (us): ");
            uart_dec(wait_time);
            wait_us(wait_time);
        }
        else
        {
            uart_puts("\n[WARNING] Frame took too long!");
        }
    }
}

void render_world_view(int camera_x, int camera_y) {
    // Clear screen or draw background
    clear_screen(0x00000000);  // Black background
    
    // Calculate which part of the map to render
    int map_start_x = camera_x;
    int map_start_y = camera_y;
    int render_width = VIEWPORT_WIDTH;
    int render_height = VIEWPORT_HEIGHT;
    
    // Clamp to map boundaries if needed
    if (map_start_x + render_width > GAME_MAP_WIDTH) {
        render_width = GAME_MAP_WIDTH - map_start_x;
    }
    if (map_start_y + render_height > GAME_MAP_HEIGHT) {
        render_height = GAME_MAP_HEIGHT - map_start_y;
    }
    
    // Draw the visible portion of the map
    drawImage_double_buffering(0, 0, 
                             game_map + map_start_y * GAME_MAP_WIDTH + map_start_x,
                             render_width, render_height);
}

void update_camera_position(int protag_x, int protag_y, int *camera_x, int *camera_y) {
    // Center camera on protagonist
    int target_x = protag_x - VIEWPORT_WIDTH/2 + PROTAG_WIDTH/2;
    int target_y = protag_y - VIEWPORT_HEIGHT/2 + PROTAG_HEIGHT/2;
    
    // Clamp camera to world boundaries
    *camera_x = target_x;
    *camera_y = target_y;
    
    if (*camera_x < 0) *camera_x = 0;
    if (*camera_y < 0) *camera_y = 0;
    if (*camera_x > WORLD_WIDTH - VIEWPORT_WIDTH) 
        *camera_x = WORLD_WIDTH - VIEWPORT_WIDTH;
    if (*camera_y > WORLD_HEIGHT - VIEWPORT_HEIGHT) 
        *camera_y = WORLD_HEIGHT - VIEWPORT_HEIGHT;
}

void update_protag_position(int *x, int *y, char direction) {
    const int step_size = RESTORE_MARGIN;
    
    switch (direction) {
        case UP:    *y -= step_size; break;
        case DOWN:  *y += step_size; break;
        case LEFT:  *x -= step_size; break;
        case RIGHT: *x += step_size; break;
    }
    
    // Clamp to world boundaries
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > WORLD_WIDTH - PROTAG_WIDTH) *x = WORLD_WIDTH - PROTAG_WIDTH;
    if (*y > WORLD_HEIGHT - PROTAG_HEIGHT) *y = WORLD_HEIGHT - PROTAG_HEIGHT;
}

// // draw sprite at a new location
// void update_protag_position(int *x, int *y, char direction)
// {
//     // const int step_size = RESTORE_MARGIN;
//     // int old_x = *x;
//     // int old_y = *y;

//     // uart_puts("\n[UPDATE_POS] Direction: ");
//     // uart_sendc(direction);
//     // uart_puts(" Old position: (");
//     // uart_dec(old_x);
//     // uart_puts(",");
//     // uart_dec(old_y);
//     // uart_puts(")");

//     // switch (direction)
//     // {
//     // case UP:
//     //     *y -= step_size;
//     //     break;
//     // case DOWN:
//     //     *y += step_size;
//     //     break;
//     // case LEFT:
//     //     *x -= step_size;
//     //     break;
//     // case RIGHT:
//     //     *x += step_size;
//     //     break;
//     // default:
//     //     uart_puts("\n[UPDATE_POS] Invalid direction!");
//     //     break;
//     // }

//     const int step_size = RESTORE_MARGIN;
//     int new_x = *x;
//     int new_y = *y;

//     switch (direction) {
//         case UP:    new_y -= step_size; break;
//         case DOWN:  new_y += step_size; break;
//         case LEFT:  new_x -= step_size; break;
//         case RIGHT: new_x += step_size; break;
//         default: uart_puts("\n[UPDATE_POS] Invalid direction!"); return; // Invalid input
//     }

//     // Clamp to screen boundaries (assuming SCREEN_WIDTH/HEIGHT are defined)
//     if (new_x < 0) new_x = 0;
//     if (new_y < 0) new_y = 0;
//     if (new_x + PROTAG_WIDTH > SCREEN_WIDTH) new_x = SCREEN_WIDTH - PROTAG_WIDTH;
//     if (new_y + PROTAG_HEIGHT > SCREEN_HEIGHT) new_y = SCREEN_HEIGHT - PROTAG_HEIGHT;

//     // Optional: Clamp to map boundaries (if map is smaller than screen)
//     if (new_x < map_x) new_x = map_x;
//     if (new_y < map_y) new_y = map_y;
//     if (new_x + PROTAG_WIDTH > map_x + GAME_MAP_WIDTH) 
//         new_x = map_x + GAME_MAP_WIDTH - PROTAG_WIDTH;
//     if (new_y + PROTAG_HEIGHT > map_y + GAME_MAP_HEIGHT) 
//         new_y = map_y + GAME_MAP_HEIGHT - PROTAG_HEIGHT;

//     *x = new_x;
//     *y = new_y;

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