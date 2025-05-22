#include "framebf.h"
#include "models/character_sprite.h"
#include "models/enemy_sprite.h"

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
void draw_item_button(int is_pressed, Character *ch);
void draw_persona_button(int is_pressed);
void draw_run_button(int is_pressed);
void draw_skill_button(int is_pressed);
void action_button();
void show_persona_selection_screen();
void screen_combat_loop();
void draw_all_buttons();
void combat_utility_UI();
void clear_persona_option_screen(); 

void draw_skill_option_screen(int persona, int option);

void wait_for_button_release_animation();

int get_enemy_x_position(int index);
int get_enemy_y_position(int index);
void draw_attack_target_screen(int target_index);

void draw_turn_indicator(CharacterSprite* sprite, int triangle_x, int triangle_y);
void draw_enemy_selected(EnemySprite *sprite, int triangle_x, int triangle_y );
typedef enum {
    SCREEN_COMBAT,
    SCREEN_PERSONA_MENU,
    SCREEN_SKILL_MENU,
    SCREEN_SELECT_ENEMY,
    SCREEN_ENEMY_COUNTER_ATTACK
} GameScreen;

extern GameScreen current_screen;  // <-- extern declaration

extern int current_player_turn;



