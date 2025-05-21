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

void game_loop();
void render_world_view(int camera_x, int camera_y);
int check_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void update_camera_position(int protag_x, int protag_y, int *camera_x, int *camera_y);
void update_protag_position(int *x, int *y, char direction);
