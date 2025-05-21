#include "../../include/game.h"
#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/gpio.h"
#include "../../include/utils.h"
#include "../../include/renderFrame.h"
#include "../../include/bitmaps/welcomeScreen.h"
#include "../../include/bitmaps/game_map.h"
#include "../../include/bitmaps/game_map_4x.h"
#include "../../include/bitmaps/protagonist_sprite.h"
#include "../../include/game_menu.h"
#include "../../include/bitmaps/enemy1.h"
#include "../../include/bitmaps/enemy2.h"
#include "../../include/bitmaps/maze1.h"
#include "../../include/bitmaps/protagonist_animation.h"
#include "../../include/game_design.h"

// --- Constants ---
#define STEP 10
#define MAX_ENEMIES 10
#define MAX_WALLS 20
#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 768
#define WORLD_WIDTH 1024*2
#define WORLD_HEIGHT 768*2
#define GAME_FRAME_US 30/1000000

#define ESCAPE 0x1B // ESC

// --- Protagonist ---
int protag_world_x = PROTAG_START_X;
int protag_world_y = PROTAG_START_Y;

// --- Camera ---
int camera_x = 0;
int camera_y = 0;

// --- Animation ---
uint32_t anim_frame = 0;
uint64_t last_anim_time = 0;
int anim_playing = 0;

Enemy enemies[MAX_ENEMIES] = {
    
    {1300, 1000, shadow1, 136, 88, 
        1, // active
        4, 4, // hitbox 
        //1 //enemy type
    },
    {1800, 450, shadow2, 68, 100, 1, 4, 4, 
        //1 //enemy type
    },
};

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

// --- Game Loop ---
void game_loop() {
    int first_frame = 1;
    char input;

    uart_puts("\n[GAME_LOOP] Starting game loop");
    uart_puts("\n[GAME_LOOP] Initial position: (");
    uart_dec(protag_world_x);
    uart_puts(",");
    uart_dec(protag_world_y);
    uart_puts(")");

    while (1) {
        uint64_t start_time = get_arm_system_time();
        uart_puts("\n[FRAME] ---- NEW FRAME ----");

        if (first_frame) {
            // Display first frame (double buffered)
            for (int i = 0; i < 2; i++) {
                render_world();
                render_protagonist_with_animation();
                swap_buffers();
                wait_us(16000);
            }
            first_frame = 0;
        } else {
            // Handle input
            input = uart_getc();
            // detect non-blocking input 
            if(input == ESCAPE){ 
                drawImage(0, 0, background, 1024, 768);
                break;
            }

            if (input) {
                anim_playing = 1;
                uart_puts("\n[INPUT]: ");
                uart_sendc(input);
            }

            // Check for lobby screen transition
            if (input == 'm' || input == 'M') {
                uart_puts("\n[INPUT] 'M' pressed - Switching to Lobby Screen...");
                lobby_screen_loop();
                first_frame = 1;
                continue;
            }

            // Update game state
            update_protagonist_position(input);
            
            // Check enemy collisions
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                
                if (check_enemy_collision(
                    protag_world_x, protag_world_y, PROTAG_WIDTH, PROTAG_HEIGHT,
                    enemies[i].world_x + enemies[i].collision_offset_x,
                    enemies[i].world_y + enemies[i].collision_offset_y,
                    enemies[i].width - 2*enemies[i].collision_offset_x,
                    enemies[i].height - 2*enemies[i].collision_offset_y
                )) {
                    uart_puts("\n[COMBAT] Enemy contact!");
                    //battle_screen_loop(enemies[i].enemy_type);
                    design_screen_loop();
                    first_frame = 1;
                    break;
                }
            }

            // Update camera
            update_camera();
            
            // Render
            render_world();
            render_protagonist_with_animation();
            swap_buffers();
        }

        // Frame timing control
        uint64_t end_time = get_arm_system_time();
        uint64_t render_time_us = ticks_to_us(end_time - start_time);
        uart_puts("\n[TIMING] Frame render time (us): ");
        uart_dec(render_time_us);

        if (render_time_us < GAME_FRAME_US) {
            uint64_t wait_time = GAME_FRAME_US - render_time_us;
            uart_puts("\n[TIMING] Waiting (us): ");
            uart_dec(wait_time);
            wait_us(wait_time);
        } else {
            uart_puts("\n[WARNING] Frame took too long!");
        }
    }
}

// --- Movement ---
void update_protagonist_position(char input) {
    int new_x = protag_world_x;
    int new_y = protag_world_y;
    
    switch (input) {
        case UP:    new_y -= STEP; break;
        case DOWN:  new_y += STEP; break;
        case LEFT:  new_x -= STEP; break;
        case RIGHT: new_x += STEP; break;
    }

    // Boundary checks
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x > WORLD_WIDTH - PROTAG_WIDTH) new_x = WORLD_WIDTH - PROTAG_WIDTH;
    if (new_y > WORLD_HEIGHT - PROTAG_HEIGHT) new_y = WORLD_HEIGHT - PROTAG_HEIGHT;

    // Wall collision checks
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

    // Only update position if no collisions
    protag_world_x = new_x;
    protag_world_y = new_y;
}

// --- Camera ---
void update_camera() {
    camera_x = protag_world_x - VIEWPORT_WIDTH / 2 + PROTAG_WIDTH / 2;
    camera_y = protag_world_y - VIEWPORT_HEIGHT / 2 + PROTAG_HEIGHT / 2;

    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x > WORLD_WIDTH - VIEWPORT_WIDTH) camera_x = WORLD_WIDTH - VIEWPORT_WIDTH;
    if (camera_y > WORLD_HEIGHT - VIEWPORT_HEIGHT) camera_y = WORLD_HEIGHT - VIEWPORT_HEIGHT;
}

// --- Collision ---
int check_enemy_collision() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        int ex = enemies[i].world_x + enemies[i].collision_offset_x;
        int ey = enemies[i].world_y + enemies[i].collision_offset_y;
        int ew = enemies[i].width - 2 * enemies[i].collision_offset_x;
        int eh = enemies[i].height - 2 * enemies[i].collision_offset_y;

        if (protag_world_x < ex + ew &&
            protag_world_x + PROTAG_WIDTH > ex &&
            protag_world_y < ey + eh &&
            protag_world_y + PROTAG_HEIGHT > ey) {
            return 1;
        }
    }
    return 0;
}

// --- Rendering ---
void render_world() {
    // Draw map
    drawImage_double_buffering_stride(
        0, 0,
        gameMap4x + camera_y * GAME_MAP_WIDTH_4X + camera_x,
        VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
        GAME_MAP_WIDTH_4X
    );

    // Draw walls
    for (int i = 0; i < MAX_WALLS; i++) {
        int screen_x = walls[i].world_x - camera_x;
        int screen_y = walls[i].world_y - camera_y;
        
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

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        int screen_x = enemies[i].world_x - camera_x;
        int screen_y = enemies[i].world_y - camera_y;
        
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

void render_protagonist_with_animation() {
    int screen_x = protag_world_x - camera_x;
    int screen_y = protag_world_y - camera_y;
    uint64_t now = get_arm_system_time();

    if (anim_playing) {
        // Advance animation every 100ms (adjust as needed)
        if (now - last_anim_time >= 100000) { 
            anim_frame = (anim_frame + 1) % protagAllArray_LEN;
            last_anim_time = now;
            if (anim_frame == 0) anim_playing = 0;
        }
        drawImage_double_buffering(screen_x, screen_y, 
            protagAllArray[anim_frame], PROTAG_WIDTH, PROTAG_HEIGHT);
    } else {
        drawImage_double_buffering(screen_x, screen_y, 
            myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
    }
}

// --- Animation ---
void start_animation() {
    anim_playing = 1;
}

// --- Battle Screen ---
void battle_screen_loop(int enemy_type) {
    uart_puts("\n[BATTLE] Entering battle screen");
    swap_buffers();
    while (1) {
        char input = uart_getc();
        if (input == 'q' || input == 'Q') {
            uart_puts("\n[BATTLE] Quitting battle");
            return;
        }
    }
}

// --- Initial Frame ---
void render_initial_frame() {
    update_camera();
    render_world();
    render_protagonist_with_animation();
    swap_buffers();
}