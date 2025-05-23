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
    int x;                   // Top-left x position in world coordinates
    int y;                   // Top-left y position in world coordinates
    const unsigned long* bitmap;  // Visual representation of the zone
    int width;               // Width of the zone
    int height;              // Height of the zone
    int target_level_number; // Level number to load when triggered
    int target_spawn_x;      // X position to spawn player in the new level
    int target_spawn_y;      // Y position to spawn player in the new level
} Zone;


typedef struct {
    int level_number;         // For progression
    const unsigned long* background; // Background image or tilemap
    int bg_width;
    int bg_height;

    Enemy* enemies;           // Array of enemies
    int enemy_count;

    Wall* walls;              // Array of walls
    int wall_count;

    Zone* zones;              // Array of transition zones
    int zone_count;

    int start_x, start_y;     // Where the player spawns in the level
} Level;

// ======================
// Batch Rendering System
// ======================

typedef struct {
    void* bitmap;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} dma_render_item;

// --- Game API ---
void game_loop();

// --- Level ---
void load_level(Level* level);  
Level* get_level_by_number(int number);      

// --- Protagonist ---
void update_protagonist_position(char input);
void update_camera(void);
void render_protagonist_with_animation(void);

// --- Rendering ---
void render_world_dma();

// --- Logic ---
int check_enemy_collision();
void battle_screen_loop(int enemy_type);
void start_animation();

// --- DMA rendering ---
void dma_render_batch(dma_render_item* items, unsigned long count, unsigned long camera_x, unsigned long camera_y);