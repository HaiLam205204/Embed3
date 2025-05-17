// Draw UI for game combat here
#include "../../include/battle_action_box.h"
#include "../../include/gpio.h"
#include "../../include/game_combat.h"
#include "../../include/renderFrame.h"
#include "../../include/uart0.h"
#include "../../include/utils.h"
#include "../../include/display_skill.h"
#include "../../include/display_persona.h"
#include "../../include/framebf.h"
#include "../../include/game_design.h"
#include "../../include/game_map.h"
#include "../../include/Orpheus_Skill_Option.h"
#include "../../include/Pixie_Skill_Option.h"

#define GAME_FRAME_RATE 30                        // e.g., 30 FPS
#define GAME_FRAME_US (1000000 / GAME_FRAME_RATE) // microseconds per frame

GameScreen current_screen = SCREEN_COMBAT;
int persona_option = 0;
int selected_persona = 0; // 0 for Orpheus, 1 for Pixie
int skill_option = 0;

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

// Draws the button in "off" or "on" state for Skill button
void draw_skill_button(int is_pressed) {
    const unsigned long *img = is_pressed ? epd_bitmap_Skill_on : epd_bitmap_Skill_off;
    drawImage_double_buffering(BUTTON_SKILL_X, BUTTON_SKILL_Y, img, BUTTON_WIDTH, BUTTON_HEIGHT);
}

void draw_persona_option_screen(int selected_option) {
    const unsigned long *img = (selected_option == 0) ?
        epd_bitmap_Display_Persona_Option1 :
        epd_bitmap_Display_Persona_Option2;

    // 1. CLEAR the screen area first (or fill a rectangle background)
    draw_rect_double_buffering(PERSONA_OPTION_SCRREN_X, PERSONA_OPTION_SCRREN_Y, 
              PERSONA_OPTION_WIDTH, PERSONA_OPTION_HEIGHT, 0xFFDDEEFF);  // Light pastel background

    // 2. DRAW the persona option image
    drawImage_double_buffering(PERSONA_OPTION_SCRREN_X,
                               PERSONA_OPTION_SCRREN_Y,
                               img,
                               PERSONA_OPTION_WIDTH,
                               PERSONA_OPTION_HEIGHT);
    // 3. SWAP the buffer to make the image show up
    // swap_buffers();
}

void draw_skill_option_screen(int persona, int option) {
    if (persona == 0) { // Orpheus
        if (option >= 0 && option < orpheus_skill_bitmap_allArray_LEN) {
            const unsigned long* img = orpheus_skill_bitmap_allArray[option];
            if (img == 0) {
                uart_puts("[ERROR] Orpheus skill bitmap is NULL\n");
            }
            draw_rect_double_buffering(ORPHEUS_SKILL_OPTION_SCRREN_X,
                ORPHEUS_SKILL_OPTION_SCRREN_Y,
                ORPHEUS_SKILL_OPTION_WIDTH,
                ORPHEUS_SKILL_OPTION_HEIGHT, 0xFFDDEEFF);

            drawImage_double_buffering(
                ORPHEUS_SKILL_OPTION_SCRREN_X,
                ORPHEUS_SKILL_OPTION_SCRREN_Y,
                img,
                ORPHEUS_SKILL_OPTION_WIDTH,
                ORPHEUS_SKILL_OPTION_HEIGHT
            );
        } 
    } else if (persona == 1) { // Pixie
        if (option >= 0 && option < pixie_skill_bitmap_allArray_LEN) {
            const unsigned long* img = pixie_skill_bitmap_allArray[option];
            if (img == 0) {
                uart_puts("[ERROR] Pixie skill bitmap is NULL\n");
            }

            draw_rect_double_buffering(PIXIE_SKILL_OPTION_SCRREN_X,
            PIXIE_SKILL_OPTION_SCRREN_Y,
            PIXIE_SKILL_OPTION_WIDTH,
            PIXIE_SKILL_OPTION_HEIGHT, 0xFFDDEEFF);

            drawImage_double_buffering(
                PIXIE_SKILL_OPTION_SCRREN_X,
                PIXIE_SKILL_OPTION_SCRREN_Y,
                img,
                PIXIE_SKILL_OPTION_WIDTH,
                PIXIE_SKILL_OPTION_HEIGHT
            );
        } 
    } 
}

void combat_utility_UI() {
    int button_pressed_attack = 0;
    int button_pressed_item = 0;
    int button_pressed_persona = 0;
    int button_pressed_run = 0;
    int button_pressed_skill = 0;
    uint64_t button_pressed_time = 0;
    // int first_frame = 1;  // Flag to show initial button once

    uart_puts("[DEBUG] Current screen: ");
    uart_dec(current_screen);
    uart_puts("\n");

    while (1) {
        uint64_t start_time = get_arm_system_time();

        // Show the "off" state initially, before any key is pressed
        draw_attack_button(button_pressed_attack);
        draw_item_button(button_pressed_item);
        draw_persona_button(button_pressed_persona);
        draw_run_button(button_pressed_run);
        draw_skill_button(button_pressed_skill);
        // first_frame = 0;
        

        // Read input
        if (uart_input_available()) {
            uart_puts("[DEBUG] Input detected\n");
            char input = uart_getc();
            if (current_screen == SCREEN_COMBAT){
                uart_puts("[DEBUG] Switched to SCREEN_COMBAT\n");
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
                    persona_option = 0;
                    draw_persona_option_screen(persona_option);
                    current_screen = SCREEN_PERSONA_MENU;
                    uart_puts("PERSONA\n");
                }
                if (input == RUN) {
                    button_pressed_run = 1;
                    button_pressed_time = start_time;
                    uart_puts("RUN\n");
                }
                if (input == SKILL) { 
                    button_pressed_skill = 1;
                    button_pressed_time = start_time;
                    skill_option = 0; // Start at top
                    draw_skill_option_screen(selected_persona, skill_option);
                    current_screen = SCREEN_SKILL_MENU;
                    uart_puts("SKILL\n");
                }
            }
            else if (current_screen == SCREEN_PERSONA_MENU){
                uart_puts("[DEBUG] Switched to SCREEN_PERSONA_MENU\n");
                draw_persona_option_screen(persona_option);
                if (input == 'o' && persona_option > 0) {
                    persona_option--;
                    draw_persona_option_screen(persona_option);
                } else if (input == 'l' && persona_option < 1) {
                    persona_option++;
                    draw_persona_option_screen(persona_option);
                } else if (input == KEY_ENTER) {  // Enter
                    selected_persona = persona_option; // <-- Save selected persona
                    // Select persona
                    current_screen = SCREEN_COMBAT;
                    button_pressed_persona = 0; // <-- ensure button state is reset 
                    redraw_combat_screen();
                    redraw_combat_screen();  
                    uart_puts("[DEBUG] Persona Confirmed, returning to combat\n");
                } 
                else if (input == KEY_ESC) {  // ESC to cancel
                    current_screen = SCREEN_COMBAT;
                    button_pressed_persona = 0; // <-- reset state
                    redraw_combat_screen(); 
                    redraw_combat_screen(); 
                    uart_puts("[DEBUG] Persona Cancelled, returning to combat\n");
                }
            }
            else if (current_screen == SCREEN_SKILL_MENU) {
                int max_skills = (selected_persona == 0) ? orpheus_skill_bitmap_allArray_LEN : pixie_skill_bitmap_allArray_LEN;

                if (input == 'o' && skill_option > 0) {
                    skill_option = (skill_option - 1 + max_skills) % max_skills;
                    draw_skill_option_screen(selected_persona, skill_option);
                } else if (input == 'l' && skill_option < max_skills - 1) {
                    skill_option = (skill_option + 1) % max_skills;
                    draw_skill_option_screen(selected_persona, skill_option);
                } else if (input == KEY_ESC) {
                    current_screen = SCREEN_COMBAT;
                    button_pressed_skill = 0;
                    redraw_combat_screen();
                    redraw_combat_screen();
                    uart_puts("[DEBUG] Skill Menu Cancelled, returning to combat\n");
                } else if (input == KEY_ENTER) {  // Enter
                    // selected_persona = persona_option; // <-- Save selected persona
                    current_screen = SCREEN_COMBAT;
                    button_pressed_persona = 0; // <-- ensure button state is reset 
                    redraw_combat_screen();
                    redraw_combat_screen();  
                    uart_puts("[DEBUG] Skill Menu Confirmed, returning to combat\n");
                }
            }
        }
        // Draw button based on state
        if (button_pressed_attack) {
            draw_attack_button(1); // Show "on"
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

        if (button_pressed_skill) {
            draw_skill_button(1);
        } else {
            draw_skill_button(0);
        }

        if (ticks_to_us(start_time - button_pressed_time) > 2000000) {
            // Reset buttons after timeout
            button_pressed_attack = 0;
            button_pressed_item = 0;
            button_pressed_persona = 0;
            button_pressed_run = 0;
            button_pressed_skill = 0;
        }

        if (current_screen == SCREEN_PERSONA_MENU) {
            draw_persona_option_screen(persona_option);
        }

        if (current_screen == SCREEN_SKILL_MENU) {
            draw_skill_option_screen(selected_persona, skill_option);
        }

        swap_buffers();
        wait_us(16000);

        // Maintain consistent frame rate
        uint64_t end_time = get_arm_system_time();
        uint64_t render_time = ticks_to_us(end_time - start_time);
        if (render_time < GAME_FRAME_US) {
            wait_us(GAME_FRAME_US - render_time);
        }
    }
}

// void clear_persona_option_screen() {
//     draw_rect_double_buffering(
//         PERSONA_OPTION_SCRREN_X,
//         PERSONA_OPTION_SCRREN_Y,
//         PERSONA_OPTION_WIDTH,
//         PERSONA_OPTION_HEIGHT,
//         0xFF000000
//     );
// }




