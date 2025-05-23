#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/game_design.h"
#include "../../include/bitmaps/game_map.h"
#include "../../include/bitmaps/protagonist_sprite.h"
#include "../../include/bitmaps/character1.h"
#include "../../include/bitmaps/character2.h"
#include "../../include/bitmaps/character3.h"
#include "../../include/bitmaps/enemy1.h"
#include "../../include/bitmaps/enemy2.h"
#include "../../include/game_combat.h"
#include "../../include/combat_character.h"
#include "../../include/models/character.h"
#include "../../include/models/enemy.h"
#include "../../include/models/character_sprite.h"
#include "../../include/models/enemy_sprite.h"
#include "../../include/models/skill.h"
//#include "../../include/bitmaps/yellow_triangle.h"
#include "../../include/game_logic.h"

// #define MAX_PROTAGONISTS 4
// #define MAX_ENEMIES 3

extern int current_player_turn;

int positions[MAX_PROTAGONISTS][2] = {
    {452, 500},
    {280, 330},
    {664, 300},
    {472, 150},
};

int enemy_positions[MAX_ENEMIES][2] = {
    {350, 320},   // Enemy 1
    {400, 370},   // Enemy 2
    {450, 320}   // Enemy 3
};

Character protagonists[MAX_PROTAGONISTS];
EnemyModel enemy[MAX_ENEMIES];

// Function prototypes
void init_protagonists();
void init_enemies();
void take_turn(int character_index);

CharacterSprite sprites[MAX_PROTAGONISTS] = {
        {&protagonists[0], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 452, 500},
        {&protagonists[1], char1, PROTAG_WIDTH, PROTAG_HEIGHT, 280, 330},
        {&protagonists[2], char2, 80, 88, 664, 300},
        {&protagonists[3], char3, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}
    };

EnemySprite enemy_sprites[MAX_ENEMIES] = {
    {&enemy[0], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 370, 270 },
    {&enemy[1], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 475, 360 },
    {&enemy[2], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 520, 270 },
};

void design_screen_loop()
{
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;
    // int turn_index = 0; // Track whose turn it is

    init_protagonists();
    init_enemies();
    while (1)
    {
        if (first_frame)
        {
            uart_puts("[FRAME] Draw background\n");
            for (int i = 0; i < 2; i++)
            {
                drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

                for (int i = 0; i < num_protagonists; ++i)
                {
                    draw_character_sprite(&sprites[i]);
                    draw_turn_indicator(&sprites[current_player_turn], 466 ,593);
                }

                for (int i = 0; i < num_enemies; ++i) {
                    draw_enemy_sprite(&enemy_sprites[i]);
                }

                swap_buffers();
            }
            first_frame = 0;
        } 

        combat_utility_UI(protagonists, num_protagonists, enemy, num_enemies);
    }

    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}

void init_protagonists()
{
    strcpy(protagonists[0].name, "Hero");
    protagonists[0].is_main_character = 1;
    protagonists[0].current_hp = 100;
    protagonists[0].max_hp = 100;

    strcpy(protagonists[1].name, "Ally1");
    protagonists[1].is_main_character = 0;
    protagonists[1].current_hp = 100;
    protagonists[1].max_hp = 100;

    strcpy(protagonists[2].name, "Ally2");
    protagonists[2].is_main_character = 0;
    protagonists[2].current_hp = 100;
    protagonists[2].max_hp = 100;

    strcpy(protagonists[3].name, "Ally3");
    protagonists[3].is_main_character = 0;
    protagonists[3].current_hp = 100;
    protagonists[3].max_hp = 100;
}

void init_enemies()
{
    strcpy(enemy[0].name, "Enemy1");
    enemy[0].current_hp = 60;
    enemy[0].max_hp = 60;
    enemy[0].enemy_type = 1; // shadow1

    strcpy(enemy[1].name, "Enemy2");
    enemy[1].current_hp = 40;
    enemy[1].max_hp = 40;
    enemy[1].enemy_type = 1; // shadow1

    strcpy(enemy[2].name, "Enemy3");
    enemy[2].current_hp = 90;
    enemy[2].max_hp = 90;
    enemy[2].enemy_type = 1; // shadow1 reused
}

void redraw_combat_screen(int current_player_turn, int selected_enemy_index)
{
    // uart_puts("[REDRAW_COMBAT_UI] Redrawing combat screen...\n");
    // uart_puts("Current player turn: ");
    uart_putint(current_player_turn);
    uart_puts("\n");
    // Draw background
    drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

    for (int i = 0; i < num_protagonists; ++i) {
        draw_character_sprite(&sprites[i]);
    }

    for (int i = 0; i < num_enemies; ++i) {
        draw_enemy_sprite(&enemy_sprites[i]);
    }

    // Redraw buttons
    draw_attack_button(0);
    draw_item_button(0);
    draw_persona_button(0);
    draw_run_button(0);
    draw_skill_button(0);


    // === Yellow triangle turn indicator ===
    CharacterSprite *current_sprite = &sprites[current_player_turn];

    int triangle_x = current_sprite->pos_x + (current_sprite->width / 2) - (40 / 2);
    int triangle_y = current_sprite->pos_y + current_sprite->height + 5; // Just below the sprite

    draw_turn_indicator(current_sprite, triangle_x, triangle_y);
    // === Highlight selected enemy if in selection screen ===
    if (current_screen == SCREEN_SELECT_ENEMY) {
        EnemySprite *selected_enemy = &enemy_sprites[selected_enemy_index];
        int enemy_triangle_x = selected_enemy->pos_x + (selected_enemy->width / 2) - (40 / 2);
        int enemy_triangle_y = selected_enemy->pos_y + selected_enemy->height + 5;
        draw_enemy_selected(selected_enemy, enemy_triangle_x, enemy_triangle_y);
    }
    // Show updated buffer
    swap_buffers();
}