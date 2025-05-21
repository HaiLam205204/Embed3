#include "framebf.h"

// Define keyboard key codes (ASCII or custom)
#define UP 0x77  // 'w'
#define LEFT 0x61  // 'a'
#define DOWN 0x73  // 's'
#define RIGHT 0x64  // 'd'

typedef struct {
    int world_x;        // World X position
    int world_y;        // World Y position
    const unsigned long* sprite;  // Sprite data
    int width;          // Sprite width
    int height;         // Sprite height
    int active;         // 1 if enemy is active/alive
    int collision_offset_x; // Fine-tune hitbox
    int collision_offset_y;
    int enemy_type; // For battle screen variety
} Enemy;

typedef struct {
    int world_x;             // World X position (top-left)
    int world_y;             // World Y position (top-left)
    const unsigned long* bitmap; // Wall sprite data
    int width;               // Wall width
    int height;              // Wall height
    int is_solid;            // 1 if collision enabled
} Wall;

typedef struct {
    const char* name;         // Optional: name or title of the level
    int level_number;         // For progression
    const unsigned long* background; // Background image or tilemap
    int bg_width;
    int bg_height;

    Enemy* enemies;           // Array of enemies
    int enemy_count;

    Wall* walls;              // Array of walls
    int wall_count;

    int start_x, start_y;     // Where the player spawns in the level
} Level;

// --- Game API ---
void game_loop();

// --- Level-related ---
void load_level(Level* level);  // Load the level: sets up enemies, walls, etc.
void clear_level();             // Optional: cleanup/reset before loading next level

// --- Protagonist ---
void update_protagonist_position(char input);
void update_camera();
void render_protagonist_with_animation();

// --- Rendering ---
void render_world();
void render_initial_frame();

// --- Logic ---
int check_enemy_collision();
void battle_screen_loop(int enemy_type);
void start_animation();