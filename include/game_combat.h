#include "framebf.h"

// Define keyboard key codes (ASCII or custom)
#define ATTACK 0x71  // 'q'
#define ITEM 0x77  // 'w'
#define PERSONA 0x65  // 'e'
#define RUN 0x72  // 'r'

void draw_attack_button(int is_pressed);
void draw_item_button(int is_pressed);
void draw_persona_button(int is_pressed);
void draw_run_button(int is_pressed);
void action_button();

