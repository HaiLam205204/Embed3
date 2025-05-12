// Draw UI for game combat here
#include "../../include/battle_action_box.h"
#include "../../include/gpio.h"
#include "../../include/game_combat.h"
#include "../../include/renderFrame.h"
#include "../../include/uart0.h"
#include "../../include/utils.h"

#define GAME_FRAME_RATE 30                        // e.g., 30 FPS
#define GAME_FRAME_US (1000000 / GAME_FRAME_RATE) // microseconds per frame
// Draws the button in "off" or "on" state
void draw_attack_button(int is_pressed)
{
    const unsigned long *img = is_pressed ? epd_bitmap_Attack_on : epd_bitmap_Attack_off;
    drawImage_double_buffering(BUTTON_ATTACK_X, BUTTON_ATTACK_Y, img, BUTTON_WIDTH, BUTTON_HEIGHT);
}

// Draws the button in "off" or "on" state for Item button
void draw_item_button(int is_pressed)
{
    const unsigned long *img = is_pressed ? epd_bitmap_Item_on : epd_bitmap_Item_off;
    drawImage_double_buffering(BUTTON_ITEM_X, BUTTON_ITEM_Y, img, BUTTON_WIDTH, BUTTON_HEIGHT);
}

// Draws the button in "off" or "on" state for Persona button
void draw_persona_button(int is_pressed)
{
    const unsigned long *img = is_pressed ? epd_bitmap_Persona_on : epd_bitmap_Persona_off;
    drawImage_double_buffering(BUTTON_PERSONA_X, BUTTON_PERSONA_Y, img, BUTTON_WIDTH, BUTTON_HEIGHT);
}

// Draws the button in "off" or "on" state for Run button
void draw_run_button(int is_pressed)
{
    const unsigned long *img = is_pressed ? epd_bitmap_Run_on : epd_bitmap_Run_off;
    drawImage_double_buffering(BUTTON_RUN_X, BUTTON_RUN_Y, img, BUTTON_WIDTH, BUTTON_HEIGHT);
}

void attack_button() {
    int button_pressed_attack = 0;
    int button_pressed_item = 0;
    int button_pressed_persona = 0;
    int button_pressed_run = 0;
    uint64_t button_pressed_time = 0;
    int first_frame = 1;  // Flag to show initial button once

    while (1) {
        uint64_t start_time = get_arm_system_time();

        // Show the "off" state initially, before any key is pressed
        if (first_frame) {
            draw_attack_button(0);
            draw_item_button(0);
            draw_persona_button(0);
            draw_run_button(0);
            first_frame = 0;
        }

        // Read input
        if (uart_input_available()) {
            char input = uart_getc();
            if (input == ATTACK) {
                button_pressed_attack = 1;
                button_pressed_time = start_time;
                uart_puts("ATTACK\n");
            }
            if (input == ITEM) {
                button_pressed_item = 1;
                button_pressed_time = start_time;
                uart_puts("ITEM\n");
            }
            if (input == PERSONA) {
                button_pressed_persona = 1;
                button_pressed_time = start_time;
                uart_puts("PERSONA\n");
            }
            if (input == RUN) {
                button_pressed_run = 1;
                button_pressed_time = start_time;
                uart_puts("RUN\n");
            }
        }

        // Draw button based on state
        if (button_pressed_attack) {
            draw_attack_button(1); // Show "on"
            // if (ticks_to_us(start_time - button_pressed_time) > 2000000) {
            //     button_pressed_attack = 0;
            // }
        } else {
            draw_attack_button(0); // Show "off"
        }

        if (button_pressed_item) {
            draw_item_button(1);  // Show "on"
        } else {
            draw_item_button(0);  // Show "off"
        }

        if (button_pressed_persona) {
            draw_persona_button(1);  // Show "on"
        } else {
            draw_persona_button(0);  // Show "off"
        }

        if (button_pressed_run) {
            draw_run_button(1);  // Show "on"
        } else {
            draw_run_button(0);  // Show "off"
        }

        if (ticks_to_us(start_time - button_pressed_time) > 2000000) {
            // Reset buttons after timeout
            button_pressed_attack = 0;
            button_pressed_item = 0;
            button_pressed_persona = 0;
            button_pressed_run = 0;
        }

        swap_buffers();

        // Maintain consistent frame rate
        uint64_t end_time = get_arm_system_time();
        uint64_t render_time = ticks_to_us(end_time - start_time);
        if (render_time < GAME_FRAME_US) {
            wait_us(GAME_FRAME_US - render_time);
        }
    }
}
