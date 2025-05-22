// Draw UI for game combat here
#include "../../../include/bitmaps/battle_action_box.h"
#include "../../../include/gpio.h"
#include "../../../include/game_combat.h"
#include "../../../include/renderFrame.h"
#include "../../../include/uart0.h"
#include "../../../include/utils.h"
#include "../../../include/display_skill.h"
#include "../../../include/bitmaps/display_persona.h"
#include "../../../include/framebf.h"
#include "../../../include/game_design.h"
#include "../../../include/bitmaps/game_map.h"
#include "../../../include/bitmaps/Orpheus_Skill_Option.h"
#include "../../../include/bitmaps/Pixie_Skill_Option.h"
#include "../../../include/models/character.h"
#include "../../../include/models/character_sprite.h"
#include "../../../include/models/enemy_sprite.h"
#include "../../../include/models/enemy.h"
#include "../../../include/game_design.h"
#include "../../../include/combat.h"
#include "../../../include/bitmaps/yellow_triangle.h"
#include "../../../include/game_logic.h"


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

void draw_turn_indicator(CharacterSprite* sprite, int triangle_x, int triangle_y ) {

    if (triangle_x == 0 && triangle_y == 0) {
        triangle_x = sprite->pos_x + (sprite->width / 2) - (TRIANGLE_WIDTH / 2);
        triangle_y = sprite->pos_y + sprite->height + 5;
    }
     // 5px below the sprite
    drawImage_double_buffering(
        triangle_x,
        triangle_y,
        epd_bitmap_triangle_turn_indicator,
        TRIANGLE_WIDTH,
        TRIANGLE_HEIGHT
    );
}

void draw_enemy_selected(EnemySprite *sprite, int triangle_x, int triangle_y ) {

    if (triangle_x == 0 && triangle_y == 0) {
        triangle_x = sprite->pos_x + (sprite->width / 2) - (TRIANGLE_WIDTH / 2);
        triangle_y = sprite->pos_y + sprite->height - 20;
    }
     // 5px below the sprite
    drawImage_double_buffering(
        triangle_x,
        triangle_y,
        epd_bitmap_triangle_turn_indicator,
        TRIANGLE_WIDTH,
        TRIANGLE_HEIGHT
    );
}

int turn_index = 0; // Track whose turn it is
extern int current_player_turn = 0; // 0 to 3 for 4 characters
int selected_enemy = 0;

void combat_utility_UI(Character protagonists[], int num_protagonists, EnemyModel enemy[], int num_enemies) {
    int button_pressed_attack = 0;
    int button_pressed_item = 0;
    int button_pressed_persona = 0;
    int button_pressed_run = 0;
    int button_pressed_skill = 0;
    // Time the last button was pressed
    uint64_t button_pressed_time = 0;
    int exit_ui = 0;  // <-- Flag to exit loop

    // turn_index = (turn_index + 1) % num_enemies;
    // current_player_turn = (current_player_turn + 1) % 4;

    uart_puts("[PLAYERS]\n");
    for (int i = 0; i < num_protagonists; ++i) {
        uart_puts(" - ");
        uart_puts(protagonists[i].name);
        uart_puts(": ");
        uart_putint(protagonists[i].current_hp);
        uart_puts("/");
        uart_putint(protagonists[i].max_hp);
        uart_puts(" HP\n");
    }

    uart_puts("[ENEMIES]\n");
    for (int i = 0; i < num_enemies; ++i) {
        uart_puts(" - ");
        uart_puts(enemy[i].name);
        uart_puts(": ");
        uart_putint(enemy[i].current_hp);
        uart_puts("/");
        uart_putint(enemy[i].max_hp);
        uart_puts(" HP\n");
    }

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
                // Character *current = &protagonists[current_player_turn];
                uart_puts("[DEBUG] Switched to SCREEN_COMBAT\n");
                if (input == ATTACK) {
                    button_pressed_attack = 1;
                    button_pressed_time = start_time;
                    uart_puts("ATTACK\n");
                    // Set current action
                    // protagonists[current_player_turn].current_action.type = ACTION_ATTACK;
                    // redraw_combat_screen(current_player_turn);
                    // redraw_combat_screen(current_player_turn);
                    // exit_ui = 1; 
                    
                    selected_enemy = 0;  // Default target
                    current_screen = SCREEN_SELECT_ENEMY;
                    redraw_combat_screen(current_player_turn, 0);
                    redraw_combat_screen(current_player_turn, 0);
                }
                if (input == ITEM) {
                    button_pressed_item = 1;
                    button_pressed_time = start_time;
                    uart_puts("ITEM\n");
                    // exit_ui = 1;
                }
                if (input == PERSONA) {
                    button_pressed_persona = 1;
                    button_pressed_time = start_time;
                    persona_option = 0;
                    draw_persona_option_screen(persona_option);
                    current_screen = SCREEN_PERSONA_MENU;
                    uart_puts("PERSONA\n");
                    // exit_ui = 1;
                }
                if (input == RUN) {
                    button_pressed_run = 1;
                    button_pressed_time = start_time;
                    uart_puts("RUN\n");
                    // exit_ui = 1;
                }
                if (input == SKILL) { 
                    button_pressed_skill = 1;
                    button_pressed_time = start_time;
                    skill_option = 0; // Start at top
                    draw_skill_option_screen(selected_persona, skill_option);
                    current_screen = SCREEN_SKILL_MENU;
                    uart_puts("SKILL\n");
                    // exit_ui = 1;
                }
                if (current_player_turn >= num_protagonists) {
                    uart_putint(current_player_turn);
                    current_player_turn = 0;
                    current_screen = SCREEN_ENEMY_COUNTER_ATTACK;
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
                    redraw_combat_screen(current_player_turn, 0);
                    redraw_combat_screen(current_player_turn, 0);  
                    uart_puts("[DEBUG] Persona Confirmed, returning to combat\n");
                } 
                else if (input == KEY_ESC) {  // ESC to cancel
                    current_screen = SCREEN_COMBAT;
                    button_pressed_persona = 0; // <-- reset state
                    redraw_combat_screen(current_player_turn, 0); 
                    redraw_combat_screen(current_player_turn, 0); 
                    uart_puts("[DEBUG] Persona Cancelled, returning to combat\n");
                }
            }
            else if (current_screen == SCREEN_SKILL_MENU) {
                int max_skills = (selected_persona == 0) ? orpheus_skill_bitmap_allArray_LEN : pixie_skill_bitmap_allArray_LEN;

                if (input == 'o' && skill_option > 0) {
                    skill_option = (skill_option - 1 + max_skills) % max_skills;
                    draw_skill_option_screen(selected_persona, skill_option);

                    // SkillSelectionResult result = handle_skill_selection();
                    // apply_skill_effect(result);
                    // // Then call take_turn() to pass to next player
                    // take_turn(turn_index);
                    // redraw_combat_screen(); 
                    // redraw_combat_screen(); 
                } else if (input == 'l' && skill_option < max_skills - 1) {
                    skill_option = (skill_option + 1) % max_skills;
                    draw_skill_option_screen(selected_persona, skill_option);

                    // SkillSelectionResult result = handle_skill_selection();
                    // apply_skill_effect(result);
                    // // Then call take_turn() to pass to next player
                    // take_turn(turn_index);
                    // redraw_combat_screen(); 
                    // redraw_combat_screen(); 
                } else if (input == KEY_ESC) {
                    current_screen = SCREEN_COMBAT;
                    button_pressed_skill = 0;
                    redraw_combat_screen(current_player_turn, 0);
                    redraw_combat_screen(current_player_turn, 0);
                    uart_puts("[DEBUG] Skill Menu Cancelled, returning to combat\n");
                } else if (input == KEY_ENTER) {  // Enter
                    // selected_persona = persona_option; // <-- Save selected persona
                    current_screen = SCREEN_COMBAT;
                    button_pressed_persona = 0; // <-- ensure button state is reset 
                    redraw_combat_screen(current_player_turn, 0);
                    redraw_combat_screen(current_player_turn, 0);  
                    uart_puts("[DEBUG] Skill Menu Confirmed, returning to combat\n");
                }
            }
            else if (current_screen == SCREEN_SELECT_ENEMY && selected_enemy >= 0) {
                int selecting = 1;
                redraw_combat_screen(current_player_turn, selected_enemy);
                redraw_combat_screen(current_player_turn, selected_enemy);
                while(selecting){
                        if (uart_input_available()) {
                        char input = uart_getc();  // <--- Get input each loop
                        if (input == 'i' && selected_enemy > 0) {
                            selected_enemy--;
                            redraw_combat_screen(current_player_turn, selected_enemy);
                            redraw_combat_screen(current_player_turn, selected_enemy);
                        } else if (input == 'p' && selected_enemy < num_enemies - 1) {
                            selected_enemy++;
                            redraw_combat_screen(current_player_turn, selected_enemy);
                            redraw_combat_screen(current_player_turn, selected_enemy);
                        } else if (input == KEY_ENTER) {
                            protagonists[current_player_turn].current_action.type = ACTION_ATTACK;
                            protagonists[current_player_turn].current_action.target_enemy = selected_enemy;
                            current_screen = SCREEN_COMBAT;
                            button_pressed_attack = 0;
                            selecting = 0;
                            exit_ui = 1;
                            current_player_turn = (current_player_turn + 1) % num_protagonists;
                            protagonists[current_player_turn].has_acted = 1;

                            int base_damage = 20;
                            deal_damage(selected_enemy, base_damage);

                            if (all_characters_have_acted(protagonists, num_protagonists)) {
                                current_screen = SCREEN_ENEMY_COUNTER_ATTACK;
                            }
                            
                            // Redraw the screen
                            redraw_combat_screen(current_player_turn, 0);
                            redraw_combat_screen(current_player_turn, 0);
                            uart_puts("[DEBUG] Attack target confirmed\n");
                        } else if (input == KEY_ESC) {
                            current_screen = SCREEN_COMBAT;
                            button_pressed_attack = 0;
                            selecting = 0;
                            redraw_combat_screen(current_player_turn, 0);
                            redraw_combat_screen(current_player_turn, 0);
                            uart_puts("[DEBUG] Attack target cancelled\n");
                        }
                    }
                }
            } else if (current_screen == SCREEN_ENEMY_COUNTER_ATTACK) {
                enemy_turn(protagonists, num_protagonists);
                reset_player_turns(protagonists, num_protagonists);
                current_screen = SCREEN_COMBAT;
                redraw_combat_screen(current_player_turn, 0);
                redraw_combat_screen(current_player_turn, 0);
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

        if (ticks_to_us(start_time - button_pressed_time) > 500000) {
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
        if (exit_ui) {
            break;
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

// void draw_attack_target_screen(int target_index) {
//     redraw_combat_screen(current_player_turn); // Reuse existing UI

//     // Draw triangle over the enemy at target_index
//     int x = get_enemy_x_position(target_index); // You define this
//     int y = get_enemy_y_position(target_index); // You define this

//     drawImage_double_buffering( x, y, epd_bitmap_triangle_turn_indicator, TRIANGLE_WIDTH,
//         TRIANGLE_HEIGHT);
// }

// int get_enemy_x_position(int index) {
//     return 250 + index * 60; // Adjust based on spacing of enemies
// }

// int get_enemy_y_position(int index) {
//     return 80; // Y position above enemy sprite
// }




