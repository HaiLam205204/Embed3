#include "framebf.h"

// Define keyboard key codes (ASCII or custom)
#define UP 0x77  // 'w'
#define LEFT 0x61  // 'a'
#define DOWN 0x73  // 's'
#define RIGHT 0x64  // 'd'

void game_loop();
void update_protag_position(int *x, int *y, char direction);
void draw_partial_map(int x, int y);
unsigned long* extract_subimage_static(const unsigned long* src, int src_width, int src_height, int start_x, int start_y, int width, int height, unsigned long* out_buffer);
