#include "game.h"
#include "framebf.h"
#include "gpio.h"
#include "utils.h"
#include "game_map.h"
#include "protagonist_sprite.h"

#define FRAME_RATE 90                 // e.g., 30 FPS
#define FRAME_US (1000000/FRAME_RATE) // microseconds per frame

void game_loop() {
    // Frame timing control (using your timer)
    set_wait_timer(1, FRAME_US);
    while (1) {
        // 1. Handle input
        // if (button_pressed(UP)) player_y--;
        // if (button_pressed(DOWN)) player_y++;
        // // ... other directions

        // 1. Handle input and update game state
        //update_protag_position(&protag_x, &protag_y);

        // 2. Render the new frame
        render_game_frame();

        // Swap buffers to display the new frame
        swap_buffers();

        // 3. Wait for next frame time
        set_wait_timer(0, 0);

        // 4. Reset timer for next frame
        set_wait_timer(1, FRAME_US);
    }    
}

void render_game_frame() {
    
    // Draw game elements (order matters - background first)
    drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);
    drawImage_double_buffering(PROTAG_START_X, PROTAG_START_Y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
}

