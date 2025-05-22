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
#include "../../include/bitmaps/transitionZone.h"
#include "../../include/game_design.h"

// --- Constants ---
#define STEP 10
#define MAX_ENEMIES 10
#define MAX_WALLS 20
#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 768
#define WORLD_WIDTH 1024*2
#define WORLD_HEIGHT 768*2
#define FRAME_RATE 30
#define GAME_FRAME_US (1000000 / FRAME_RATE)
#define MAX_LEVELS 3

#define NULL ((void*)0)
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

// --- Level 1 Enemies ---
Enemy level1_enemies[MAX_ENEMIES] = {
    {1300, 1000, shadow1, 136, 88, 
        1, // active
        4, 4, // hitbox 
        //1 //enemy type
    },
    {1800, 450, shadow2, 68, 100, 1, 4, 4, 
        //1 //enemy type
    },
};

// --- Level 2 Enemies ---
Enemy level2_enemies[MAX_ENEMIES] = {
    {500, 300, shadow1, 136, 88, 1, 4, 4},
    {900, 700, shadow2, 68, 100, 1, 4, 4},
    {1500, 1200, shadow2, 68, 100, 1, 4, 4},
};

// --- Level 3 Enemies ---
Enemy level3_enemies[MAX_ENEMIES] = {
    {800, 600, shadow1, 136, 88, 1, 4, 4},
};

// --- Level 1 Walls ---
Wall level1_walls[MAX_WALLS] = {
    // {WALL1_START_X, WALL1_START_Y, wall1, WALL1_WIDTH, WALL1_HEIGHT, 1}, 
    // {WALL2_START_X, WALL2_START_Y, wall2, WALL2_WIDTH, WALL2_HEIGHT, 1}, 
    // {WALL1_START_X, WALL1_START_Y + 768*2 - 63, wall1, WALL1_WIDTH, WALL1_HEIGHT, 1}, 
    // {WALL2_START_X + 1024*2 - 63, WALL2_START_Y, wall2, WALL2_WIDTH, WALL2_HEIGHT, 1},
    {WALL3_START_X, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*2, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*3, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*5, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*2, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*3, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*5, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    // Add more walls...
};

// --- Level 2 Walls ---
Wall level2_walls[MAX_WALLS] = {
    {WALL3_START_X, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*2, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*3, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*5, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},

    {WALL3_START_X + 342*3, WALL3_START_Y + 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*4, WALL3_START_Y + 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*4, WALL3_START_Y - 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*3, WALL3_START_Y - 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},

    {WALL4_START_X + 342, WALL4_START_Y + 256, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y + 256*2, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y + 256*3, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y + 256*4, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},

    {WALL4_START_X - 342, WALL4_START_Y + 256*5, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X - 342, WALL4_START_Y + 256*4, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X - 342, WALL4_START_Y, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X - 342, WALL4_START_Y + 256, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
};

// --- Level 3 Walls ---
Wall level3_walls[MAX_WALLS] = {
    {WALL3_START_X + 342, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*2, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*5, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*4, WALL3_START_Y, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},

    {WALL3_START_X, WALL3_START_Y + 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},
    {WALL3_START_X + 342*3, WALL3_START_Y - 256*2, wall3, WALL3_WIDTH, WALL3_HEIGHT, 1},

    {WALL4_START_X, WALL4_START_Y + 256, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*2, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*3, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X, WALL4_START_Y + 256*4, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X - 342*2, WALL4_START_Y, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X - 342*2, WALL4_START_Y + 256, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y + 256*5, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
    {WALL4_START_X + 342, WALL4_START_Y + 256, wall4, WALL4_WIDTH, WALL4_HEIGHT, 1},
};

Zone level1_zones[] = {
    { .x = 0, .y = 1408, .bitmap = zone, .width = ZONE_WIDTH, .height = ZONE_HEIGHT, .target_level_number = 2, .target_spawn_x = 0, .target_spawn_y = 0 }
};

Zone level2_zones[] = {
    { .x = 0, .y = 1408, .bitmap = zone, .width = ZONE_WIDTH, .height = ZONE_HEIGHT, .target_level_number = 3, .target_spawn_x = 0, .target_spawn_y = 0 }
};

Zone level3_zones[] = {
    { .x = 1920, .y = 0, .bitmap = zone, .width = ZONE_WIDTH, .height = ZONE_HEIGHT, .target_level_number = 1, .target_spawn_x = 0, .target_spawn_y = 0 }
};

// --- Levels ---
Level levels[MAX_LEVELS] = {
    {
        .level_number = 1,
        .background = gameMap4x,
        .bg_width = GAME_MAP_WIDTH_4X,
        .bg_height = GAME_MAP_HEIGHT_4X,
        .enemies = level1_enemies,
        .enemy_count = sizeof(level1_enemies) / sizeof(Enemy),
        .walls = level1_walls,
        .wall_count = sizeof(level1_walls) / sizeof(Wall),
        .zones = level1_zones,
        .zone_count = 1,
        .start_x = 100,
        .start_y = 100,
    },
    {
        .level_number = 2,
        .background = gameMap4x, // Change if using another map
        .bg_width = GAME_MAP_WIDTH_4X,
        .bg_height = GAME_MAP_HEIGHT_4X,
        .enemies = level2_enemies,
        .enemy_count = sizeof(level2_enemies) / sizeof(Enemy),
        .walls = level2_walls,
        .wall_count = sizeof(level2_walls) / sizeof(Wall),
        .zones = level2_zones,
        .zone_count = 1,
        .start_x = 200,
        .start_y = 300,
    },
    {
        .level_number = 3,
        .background = gameMap4x, // Update if you add unique backgrounds
        .bg_width = GAME_MAP_WIDTH_4X,
        .bg_height = GAME_MAP_HEIGHT_4X,
        .enemies = level3_enemies,
        .enemy_count = sizeof(level3_enemies) / sizeof(Enemy),
        .walls = level3_walls,
        .wall_count = sizeof(level3_walls) / sizeof(Wall),
        .zones = level3_zones,
        .zone_count = 1,
        .start_x = 50,
        .start_y = 600,
    }
};

const int total_levels = sizeof(levels) / sizeof(Level);
Level* current_level = &levels[0]; // Start at level 1

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
            //Display first frame (double buffered)
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
                draw_background();
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
            for (int i = 0; i < current_level->enemy_count; i++) {
                Enemy* enemy = &current_level->enemies[i];
                if (!enemy->active) continue;

                if (check_enemy_collision(
                    protag_world_x, protag_world_y, PROTAG_WIDTH, PROTAG_HEIGHT,
                    enemy->world_x + enemy->collision_offset_x,
                    enemy->world_y + enemy->collision_offset_y,
                    enemy->width - 2 * enemy->collision_offset_x,
                    enemy->height - 2 * enemy->collision_offset_y
                )) {
                    uart_puts("\n[COMBAT] Enemy contact!");
                    // battle_screen_loop(enemy->enemy_type);
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

    // Boundary checks against level dimensions
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;
    if (new_x > current_level->bg_width - PROTAG_WIDTH)
        new_x = current_level->bg_width - PROTAG_WIDTH;
    if (new_y > current_level->bg_height - PROTAG_HEIGHT)
        new_y = current_level->bg_height - PROTAG_HEIGHT;

    // Wall collision checks
    for (int i = 0; i < current_level->wall_count; i++) {
        Wall* wall = &current_level->walls[i];
        if (!wall->is_solid) continue;

        if (new_x < wall->world_x + wall->width &&
            new_x + PROTAG_WIDTH > wall->world_x &&
            new_y < wall->world_y + wall->height &&
            new_y + PROTAG_HEIGHT > wall->world_y) {
            uart_puts("\n[COLLISION] Blocked by wall");
            return; // Cancel movement
        }
    }

    // Zone transition check
    for (int i = 0; i < current_level->zone_count; i++) {
        Zone* zone = &current_level->zones[i];

        if (new_x < zone->x + zone->width &&
            new_x + PROTAG_WIDTH > zone->x &&
            new_y < zone->y + zone->height &&
            new_y + PROTAG_HEIGHT > zone->y) {

            uart_puts("\n[ZONE] Transition triggered");

            // Load new level
            Level* new_level = get_level_by_number(zone->target_level_number); 
            load_level(new_level);

            // Set protagonist to target spawn
            protag_world_x = zone->target_spawn_x;
            protag_world_y = zone->target_spawn_y;

            update_camera(); // Realign camera after teleport

            return; // Exit early — no need to update with new_x/y
        }
    }

    // Update position if no collisions
    protag_world_x = new_x;
    protag_world_y = new_y;
}

// --- Level tranisition ---
void load_level(Level* level) {
    if (!level) return;

    uart_puts("\n[LEVEL] Loading level ");
    uart_dec(level->level_number);

    // Set current level
    current_level = level;

    // Reset protagonist position to level's start position
    protag_world_x = level->start_x;
    protag_world_y = level->start_y;

    // Reset camera
    update_camera();

    // Activate all enemies and walls in the level
    for (int i = 0; i < level->enemy_count; i++) {
        level->enemies[i].active = 1;
    }
    for (int i = 0; i < level->wall_count; i++) {
        level->walls[i].is_solid = 1;
    }

    // Double buffered render of initial level state
    for (int i = 0; i < 2; i++) {

        // Draw background
        drawImage_double_buffering_stride(
            0, 0,
            level->background + camera_y * level->bg_width + camera_x,
            VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
            level->bg_width
        );

        // Draw walls
        for (int j = 0; j < level->wall_count; j++) {
            Wall* wall = &level->walls[j];
            int screen_x = wall->world_x - camera_x;
            int screen_y = wall->world_y - camera_y;
            
            if (screen_x + wall->width > 0 && screen_x < VIEWPORT_WIDTH &&
                screen_y + wall->height > 0 && screen_y < VIEWPORT_HEIGHT) {
                drawImage_double_buffering(
                    screen_x, screen_y,
                    wall->bitmap,
                    wall->width, wall->height
                );
            }
        }

        // Draw protagonist
        drawImage_double_buffering(
            protag_world_x - camera_x,
            protag_world_y - camera_y,
            myBitmapprotag,  // Default protagonist sprite
            PROTAG_WIDTH,
            PROTAG_HEIGHT
        );

        swap_buffers();
        wait_us(16000); // Small delay for smooth transition
    }

    uart_puts("\n[LEVEL] Loaded successfully");
}

Level* get_level_by_number(int number) {
    for (int i = 0; i < total_levels; i++) {
        if (levels[i].level_number == number) {
            return &levels[i];
        }
    }

    uart_puts("\n[ERROR] Level not found: ");
    uart_dec(number);
    return NULL;
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
    for (int i = 0; i < current_level->enemy_count; i++) {
        Enemy* enemy = &current_level->enemies[i];
        if (!enemy->active) continue;

        int ex = enemy->world_x + enemy->collision_offset_x;
        int ey = enemy->world_y + enemy->collision_offset_y;
        int ew = enemy->width - 2 * enemy->collision_offset_x;
        int eh = enemy->height - 2 * enemy->collision_offset_y;

        if (protag_world_x < ex + ew &&
            protag_world_x + PROTAG_WIDTH > ex &&
            protag_world_y < ey + eh &&
            protag_world_y + PROTAG_HEIGHT > ey) {
            return 1; // Collision detected
        }
    }
    return 0; // No collision
}


// --- Rendering ---
void render_world() {
    if (current_level == NULL) return;

    // Draw map
    drawImage_double_buffering_stride(
        0, 0,
        gameMap4x + camera_y * GAME_MAP_WIDTH_4X + camera_x,
        VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
        GAME_MAP_WIDTH_4X
    );

    // --- Draw walls ---
    for (int i = 0; i < current_level->wall_count; i++) {
        Wall* wall = &current_level->walls[i];

        int screen_x = wall->world_x - camera_x;
        int screen_y = wall->world_y - camera_y;

        if (screen_x + wall->width > 0 && screen_x < VIEWPORT_WIDTH &&
            screen_y + wall->height > 0 && screen_y < VIEWPORT_HEIGHT) {
            drawImage_double_buffering(
                screen_x, screen_y,
                wall->bitmap,
                wall->width, wall->height
            );
        }
    }

    // --- Draw enemies ---
    for (int i = 0; i < current_level->enemy_count; i++) {
        Enemy* enemy = &current_level->enemies[i];
        if (!enemy->active) continue;

        int screen_x = enemy->world_x - camera_x;
        int screen_y = enemy->world_y - camera_y;

        if (screen_x + enemy->width > 0 && screen_x < VIEWPORT_WIDTH &&
            screen_y + enemy->height > 0 && screen_y < VIEWPORT_HEIGHT) {
            drawImage_double_buffering(
                screen_x, screen_y,
                enemy->sprite,
                enemy->width, enemy->height
            );
        }
    }

    // --- Draw zones ---
    for (int i = 0; i < current_level->zone_count; i++) {
        Zone* zone = &current_level->zones[i];
        if (!zone->bitmap) continue;

        int screen_x = zone->x - camera_x;
        int screen_y = zone->y - camera_y;

        if (screen_x + zone->width > 0 && screen_x < VIEWPORT_WIDTH &&
            screen_y + zone->height > 0 && screen_y < VIEWPORT_HEIGHT) {
            drawImage_double_buffering(
                screen_x, screen_y,
                zone->bitmap,
                zone->width, zone->height
            );
        }
    }
}

// --- Draw walking animation ---
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
