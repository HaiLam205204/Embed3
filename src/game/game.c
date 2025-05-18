#include "../../include/game.h"
#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/gpio.h"
#include "../../include/utils.h"
#include "../../include/renderFrame.h"
#include "../../include/bitmaps/game_map.h"
#include "../../include/bitmaps/game_map_4x.h"
#include "../../include/bitmaps/protagonist_sprite.h"
#include "../../include/game_menu.h"
#include "../../include/bitmaps/enemy1.h"
#include "../../include/bitmaps/enemy2.h"
#include "../../include/bitmaps/maze1.h"

#define GAME_FRAME_RATE 30                        // 30 FPS
#define GAME_FRAME_US (1000000 / GAME_FRAME_RATE) // microseconds per 

#define NULL ((void *)0)

#define VIEWPORT_WIDTH  1024  // Physical screen width
#define VIEWPORT_HEIGHT 768  // Physical screen height
#define WORLD_WIDTH     GAME_MAP_WIDTH_4X  // Map is twice as big as screen
#define WORLD_HEIGHT    GAME_MAP_HEIGHT_4X 

#define RESTORE_MARGIN 10
// The safety margin (RESTORE_MARGIN) ensures complete coverage of the changed pixels
static unsigned long sprite_bg_buffer[(PROTAG_WIDTH + 2 * RESTORE_MARGIN) * (PROTAG_HEIGHT + 2 * RESTORE_MARGIN)];

// map starting coordinates
int map_x = MAP_4X_START_X;
int map_y = MAP_4X_START_Y;

// Camera offset
int camera_x = 0;
int camera_y = 0;

// World coordinates of sprite
int protag_world_x = PROTAG_START_X;  
int protag_world_y = PROTAG_START_Y;

#define MAX_ENEMIES 10

Enemy enemies[MAX_ENEMIES] = {
    {1300, 1000, shadow1, 136, 88, 1, 4, 4, 
        //1 //enemy type
    },  
    {1800, 450, shadow2, 68, 100, 1, 4, 4, 
        //1 //enemy type
    },
    // Add more enemies as needed
};

#define MAX_WALLS 20

Wall walls[MAX_WALLS] = {
    {MAZE1_START_X, MAZE1_START_Y, maze1, MAZE1_WIDTH, MAZE1_HEIGHT, 1},  // First maze
    // Add more walls...
};

void game_loop()
{
    int first_frame = 1;
    char input;

    uart_puts("\n[GAME_LOOP] Starting game loop");
    uart_puts("\n[GAME_LOOP] Initial position: (");
    uart_dec(protag_world_x);
    uart_puts(",");
    uart_dec(protag_world_y);
    uart_puts(")");

    while (1)
    {
        uint64_t start_time = get_arm_system_time();
        uart_puts("\n[FRAME] ---- NEW FRAME ----");

        if (first_frame)
        { // display first frame
            for (int i = 0; i < 2; i++)
            {
                // draw game map
                drawImage_double_buffering(map_x, map_y, gameMap4x, GAME_MAP_WIDTH_4X, GAME_MAP_HEIGHT_4X);

                // draw protagonist
                drawImage_double_buffering(protag_world_x, protag_world_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);

                // swap buffer to display frame
                swap_buffers();
                uart_puts("\n[FRAME] Swapped buffers");
                wait_us(16000);
            }
            first_frame = 0; // toggle flag
        }
        else
        { // display second frame onwards
            input = uart_getc();

            // Check for 'M' key to jump to lobby screen
            if (input == 'm' || input == 'M') {
                uart_puts("\n[INPUT] 'M' pressed - Switching to Lobby Screen...");
                lobby_screen_loop(); // Call lobby loop
                // After lobby screen ends, redraw the current game frame again
                first_frame = 1;
                continue;  // Continue the loop to redraw the game frame
            }

            // Update player position on the map
            update_protag_position(&protag_world_x, &protag_world_y, input);

            // Check for enemies encounters
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                
                if (check_collision(
                    protag_world_x, protag_world_y, PROTAG_WIDTH, PROTAG_HEIGHT,
                    enemies[i].world_x + enemies[i].collision_offset_x,
                    enemies[i].world_y + enemies[i].collision_offset_y,
                    enemies[i].width - 2*enemies[i].collision_offset_x,
                    enemies[i].height - 2*enemies[i].collision_offset_y
                )) {
                    uart_puts("\n[COMBAT] Enemy contact!");

                    // Transition to battle
                    //clear_screen(0xFFFF0000);
                    battle_screen_loop(enemies[i].enemy_type);
                    first_frame = 1; // Reset game frame after battle
                    break; // Exit combat loop
                }
            }
            
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

void battle_screen_loop(int enemy_type) {
    uart_puts("\n[BATTLE] Starting battle screen");
    
    // DRAW HERE
    //drawString(300, 300, "BATTLE MODE!", 0xFFFFFF);
    swap_buffers();
    
    // Simple battle loop
    while (1) {
        char input = uart_getc();
        if (input == 'q' || input == 'Q') {
            uart_puts("\n[BATTLE] Exiting battle");
            return;
        }
    }
}

int check_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}

void render_world_view(int camera_x, int camera_y) {
    
    // Calculate which part of the map to render
    int map_start_x = camera_x;
    int map_start_y = camera_y;
    int render_width = VIEWPORT_WIDTH;
    int render_height = VIEWPORT_HEIGHT;
    
    // Clamp to map boundaries if needed
    if (map_start_x + render_width > GAME_MAP_WIDTH_4X) {
        render_width = GAME_MAP_WIDTH_4X - map_start_x;
    }
    if (map_start_y + render_height > GAME_MAP_HEIGHT_4X) {
        render_height = GAME_MAP_HEIGHT_4X - map_start_y;
    }

    drawImage_double_buffering_stride(0, 0, 
                                gameMap4x + map_start_y * GAME_MAP_WIDTH_4X + map_start_x,
                                render_width, render_height,
                                GAME_MAP_WIDTH_4X);  // STRIDE

    // // Render walls in viewport
    // for (int i = 0; i < MAX_WALLS; i++) {
    //     int screen_x = walls[i].world_x - camera_x;
    //     int screen_y = walls[i].world_y - camera_y;

    //     // Check if wall intersects viewport
    //     if (screen_x + walls[i].width > 0 && 
    //         screen_x < VIEWPORT_WIDTH &&
    //         screen_y + walls[i].height > 0 && 
    //         screen_y < VIEWPORT_HEIGHT) {
            
    //         drawImage_double_buffering(screen_x, screen_y, 
    //             walls[i].bitmap, 
    //             walls[i].width, 
    //             walls[i].height);
    //     }
    // }

    // Render enemies that are in view
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        // Calculate screen coordinates
        int screen_x = enemies[i].world_x - camera_x;
        int screen_y = enemies[i].world_y - camera_y;
        
        // Check if enemy is in viewport
        if (screen_x + enemies[i].width > 0 && 
            screen_x < VIEWPORT_WIDTH &&
            screen_y + enemies[i].height > 0 && 
            screen_y < VIEWPORT_HEIGHT) {
            
            drawImage_double_buffering(screen_x, screen_y, 
                enemies[i].sprite, 
                enemies[i].width, 
                enemies[i].height);
        }
    }
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
    // int new_x = *x, new_y = *y;
    
    switch (direction) {
        case UP:    *y -= step_size; break;
        case DOWN:  *y += step_size; break;
        case LEFT:  *x -= step_size; break;
        case RIGHT: *x += step_size; break;
    }
    
    // // Check wall collisions
    // for (int i = 0; i < MAX_WALLS; i++) {
    //     if (!walls[i].is_solid) continue;
        
    //     if (new_x < walls[i].world_x + walls[i].width &&
    //         new_x + PROTAG_WIDTH > walls[i].world_x &&
    //         new_y < walls[i].world_y + walls[i].height &&
    //         new_y + PROTAG_HEIGHT > walls[i].world_y) {
    //         return; // Cancel movement if collision
    //     }
    // }

    // // Update position if no collision
    // *x = new_x;
    // *y = new_y;

    // Clamp to world boundaries
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > WORLD_WIDTH - PROTAG_WIDTH) *x = WORLD_WIDTH - PROTAG_WIDTH;
    if (*y > WORLD_HEIGHT - PROTAG_HEIGHT) *y = WORLD_HEIGHT - PROTAG_HEIGHT;
}

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
    if (map_local_x + restore_width > GAME_MAP_WIDTH_4X)
    {
        restore_width = GAME_MAP_WIDTH_4X - map_local_x;
    }
    if (map_local_y + restore_height > GAME_MAP_HEIGHT_4X)
    {
        restore_height = GAME_MAP_HEIGHT_4X - map_local_y;
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
        gameMap4x, GAME_MAP_WIDTH_4X, GAME_MAP_HEIGHT_4X,
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