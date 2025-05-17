#include "framebf.h"

// Define keyboard key codes (ASCII or custom)
#define ATTACK 0x71  // 'q'
#define ITEM 0x77  // 'w'
#define PERSONA 0x65  // 'e'
#define RUN 0x72  // 'r'
#define SKILL 0x74 // 't'

#define KEY_ARROW_UP    0x6F  // ANSI escape sequence for arrow up
#define KEY_ARROW_DOWN  0x6C  // ANSI escape sequence for arrow down
#define KEY_ENTER       0x0A      // Optional: confirm selection
#define KEY_ESC         0x1B      // quit selection scrren

void draw_attack_button(int is_pressed);
void draw_item_button(int is_pressed);
void draw_persona_button(int is_pressed);
void draw_run_button(int is_pressed);
void draw_skill_button(int is_pressed);
void action_button();
void show_persona_selection_screen();
void screen_combat_loop();
void draw_all_buttons();
void combat_utility_UI();
void clear_persona_option_screen(); 

typedef enum {
    SCREEN_COMBAT,
    SCREEN_PERSONA_MENU
} GameScreen;



