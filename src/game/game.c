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
#include "../../include/bitmaps/protagonist_animation.h"

#define GAME_FRAME_RATE 30                        // 30 FPS
#define GAME_FRAME_US (1000000 / GAME_FRAME_RATE) // microseconds per 

#define NULL ((void *)0)

#define VIEWPORT_WIDTH  1024  // Physical screen width
#define VIEWPORT_HEIGHT 768  // Physical screen height
#define WORLD_WIDTH     GAME_MAP_WIDTH_4X  // Map is twice as big as screen
#define WORLD_HEIGHT    GAME_MAP_HEIGHT_4X 

// map starting coordinates
int map_x = MAP_4X_START_X;
int map_y = MAP_4X_START_Y;

// Camera offset
int camera_x = 0;
int camera_y = 0;

// World coordinates of sprite
int protag_world_x = PROTAG_START_X;  
int protag_world_y = PROTAG_START_Y;

// Static state variables
static uint32_t animation_frame = 0;
static uint64_t last_anim_time = 0;
static int playing_animation = 0;

#define MAX_ENEMIES 10

Enemy enemies[MAX_ENEMIES] = {
    {1300, 1000, shadow1, 136, 88, 1, 4, 4, 
        //1 //enemy type
    },  
    {1800, 450, shadow2, 68, 100, 1, 4, 4, 
        //1 //enemy type
    },
    // Add more enemies...
};

#define MAX_WALLS 20

Wall walls[MAX_WALLS] = {
    {WALL1_START_X, WALL1_START_Y, wall1, WALL1_WIDTH, WALL1_HEIGHT, 1}, 
    {WALL2_START_X, WALL2_START_Y, wall2, WALL2_WIDTH, WALL2_HEIGHT, 1}, 
    {WALL1_START_X, WALL1_START_Y + 768*2 - 63, wall1, WALL1_WIDTH, WALL1_HEIGHT, 1}, 
    {WALL2_START_X + 1024*2 - 63, WALL2_START_Y, wall2, WALL2_WIDTH, WALL2_HEIGHT, 1},
    {WALL3_START_X, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*2, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*3, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*5, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*2, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*3, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*5, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
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

                // Render walls in viewport
                for (int i = 0; i < MAX_WALLS; i++) {
                    int screen_x = walls[i].world_x - camera_x;
                    int screen_y = walls[i].world_y - camera_y;

                    // Check if wall intersects viewport
                    if (screen_x + walls[i].width > 0 && 
                        screen_x < VIEWPORT_WIDTH &&
                        screen_y + walls[i].height > 0 && 
                        screen_y < VIEWPORT_HEIGHT) {
                        
                        drawImage_double_buffering(screen_x, screen_y, 
                            walls[i].bitmap, 
                            walls[i].width, 
                            walls[i].height);
                    }
                }

                // swap buffer to display frame
                swap_buffers();
                wait_us(16000);
            }
            first_frame = 0; // toggle flag                  
        }
        else
        { // update second frame onwards
            input = uart_getc();
            if (input) {
                playing_animation = 1;
            }
            uart_puts("\n[INPUT]: ");
            uart_sendc(input);

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
            // At the start of the frame
            uint64_t now = get_arm_system_time();
            int protag_screen_x = protag_world_x - camera_x;
            int protag_screen_y = protag_world_y - camera_y;
            //drawImage_double_buffering(protag_screen_x, protag_screen_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);

            // ✅ Only draw one version of protagonist
            if (playing_animation) {
                if (now - last_anim_time >= GAME_FRAME_US) {
                    drawImage_double_buffering(
                        protag_screen_x, protag_screen_y,
                        protagAllArray[animation_frame],
                        PROTAG_WIDTH, PROTAG_HEIGHT
                    );
                    animation_frame++;
                    last_anim_time = now;

                    if (animation_frame >= protagAllArray_LEN) {
                        animation_frame = 0;
                        playing_animation = 0;
                    }
                } else {
                    // Optionally draw last frame again or skip
                    drawImage_double_buffering(
                        protag_screen_x, protag_screen_y,
                        protagAllArray[animation_frame],
                        PROTAG_WIDTH, PROTAG_HEIGHT
                    );
                }
            } else {
                drawImage_double_buffering(
                    protag_screen_x, protag_screen_y,
                    myBitmapprotag,
                    PROTAG_WIDTH, PROTAG_HEIGHT
                );
            }
            
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

    // Render walls in viewport
    for (int i = 0; i < MAX_WALLS; i++) {
        int screen_x = walls[i].world_x - camera_x;
        int screen_y = walls[i].world_y - camera_y;

        // Check if wall intersects viewport
        if (screen_x + walls[i].width > 0 && 
            screen_x < VIEWPORT_WIDTH &&
            screen_y + walls[i].height > 0 && 
            screen_y < VIEWPORT_HEIGHT) {
            
            drawImage_double_buffering(screen_x, screen_y, 
                walls[i].bitmap, 
                walls[i].width, 
                walls[i].height);
        }
    }

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
    const int step_size = 10;
    int new_x = *x;
    int new_y = *y;
    
    // Apply movement to the temporary variables
    switch (direction) {
        case UP:    new_y -= step_size; break;
        case DOWN:  new_y += step_size; break;
        case LEFT:  new_x -= step_size; break;
        case RIGHT: new_x += step_size; break;
    }

    // Clamp proposed new position
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x > WORLD_WIDTH - PROTAG_WIDTH) new_x = WORLD_WIDTH - PROTAG_WIDTH;
    if (new_y > WORLD_HEIGHT - PROTAG_HEIGHT) new_y = WORLD_HEIGHT - PROTAG_HEIGHT;

    // Check wall collisions with the proposed new position
    for (int i = 0; i < MAX_WALLS; i++) {
        if (!walls[i].is_solid) continue;

        if (new_x < walls[i].world_x + walls[i].width &&
            new_x + PROTAG_WIDTH > walls[i].world_x &&
            new_y < walls[i].world_y + walls[i].height &&
            new_y + PROTAG_HEIGHT > walls[i].world_y) {
            uart_puts("\n[COLLISION] Blocked by wall");
            return; // Cancel movement
        }
    }

    // Update only if no collision
    *x = new_x;
    *y = new_y;
}