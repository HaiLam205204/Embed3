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

#define MAX_PROTAGONISTS 4
#define MAX_ENEMIES 3

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
Enemy enemy[MAX_ENEMIES];

// Function prototypes
void init_protagonists();
void init_enemies();
void take_turn(int character_index);

void design_screen_loop()
{
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;
    int turn_index = 0; // Track whose turn it is

    init_protagonists();
    init_enemies();
    CharacterSprite sprites[MAX_PROTAGONISTS] = {
        {&protagonists[0], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 452, 500},
        {&protagonists[1], char1, PROTAG_WIDTH, PROTAG_HEIGHT, 280, 330},
        {&protagonists[2], char2, 80, 88, 664, 300},
        {&protagonists[3], char3, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}
    };

    EnemySprite enemies_sprites[MAX_ENEMIES] = {
        {&enemy[0], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 370, 320 },
        {&enemy[1], shadow2, ENEMY2_WIDTH, ENEMY2_HEIGHT, 445, 370 },
        {&enemy[2], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 520, 320 },
    };

    while (1)
    {
        if (first_frame)
        {
            uart_puts("[FRAME] Draw background\n");
            for (int i = 0; i < 2; i++)
            {
                drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

                for (int i = 0; i < MAX_PROTAGONISTS; ++i)
                {
                    draw_character_sprite(&sprites[i]);
                }

                for (int i = 0; i < MAX_ENEMIES; ++i) {
                    draw_enemy_sprite(&enemies_sprites[i]);
                }

                swap_buffers();
            }
            first_frame = 0;
        } 

        combat_utility_UI();
        // Simple turn iteration
        uart_puts("[TURN] It's ");
        uart_puts(protagonists[turn_index].name);
        uart_puts("'s turn!\n");

        take_turn(turn_index);

        // Move to the next character, looping around
        turn_index = (turn_index + 1) % MAX_PROTAGONISTS;

        // Bỏ mấy cái action logic vào
        uart_puts("Press any key to proceed to the next turn...\n");
        uart_getc();
    }

    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}

void take_turn(int character_index)
{
    Character *character = &protagonists[character_index];
    uart_puts("[ACTION] ");
    uart_puts(character->name);
    uart_puts(" takes a simple action.\n");
    int selected_enemy = 0;

    // Display enemy names
    uart_puts("[SELECT TARGET] Use 'y' to go left, 'u' to go right. Press Enter to confirm.\n");
    while (1)
    {
        uart_puts("[SELECTED ENEMY] ");
        uart_puts(enemy[selected_enemy].name);
        uart_puts("\n");

        char input = uart_getc();
        if (input == '\r' || input == '\n')  // Confirm selection with Enter
            break;

        if (input == 'y') // Left
        {
            selected_enemy = (selected_enemy - 1 + MAX_ENEMIES) % MAX_ENEMIES;
        }
        else if (input == 'u') // Right
        {
            selected_enemy = (selected_enemy + 1) % MAX_ENEMIES;
        }
    }

    // Apply base damage
    int base_damage = 30;
    Enemy *target = &enemy[selected_enemy];

    uart_puts("[ATTACK] ");
    uart_puts(character->name);
    uart_puts(" attacks ");
    uart_puts(target->name);
    uart_puts(" for 30 damage!\n");

    target->current_hp -= base_damage;
    if (target->current_hp < 0)
        target->current_hp = 0;

    // Log enemy HP after attack
    uart_puts("[RESULT] ");
    uart_puts(target->name);
    uart_puts(" HP is now ");
    uart_puts(target->current_hp);
    uart_puts(" / ");
    uart_puts(target->max_hp);
    uart_puts("\n");
}

void init_protagonists()
{
    strcpy(protagonists[0].name, "Hero");
    protagonists[0].is_main_character = 1;
    protagonists[0].current_hp = 100;
    protagonists[0].max_hp = 100;

    strcpy(protagonists[1].name, "Ally1");
    protagonists[1].is_main_character = 0;
    protagonists[1].current_hp = 80;
    protagonists[1].max_hp = 100;

    strcpy(protagonists[2].name, "Ally2");
    protagonists[2].is_main_character = 0;
    protagonists[2].current_hp = 50;
    protagonists[2].max_hp = 100;

    strcpy(protagonists[3].name, "Ally3");
    protagonists[3].is_main_character = 0;
    protagonists[3].current_hp = 20;
    protagonists[3].max_hp = 100;
}

void init_enemies()
{
    strcpy(enemy[0].name, "Enemy1");
    enemy[0].current_hp = 30;
    enemy[0].max_hp = 60;
    enemy[0].enemy_type = 1; // shadow1

    strcpy(enemy[1].name, "Enemy2");
    enemy[1].current_hp = 40;
    enemy[1].max_hp = 40;
    enemy[1].enemy_type = 2; // shadow2

    strcpy(enemy[2].name, "Enemy3");
    enemy[2].current_hp = 90;
    enemy[2].max_hp = 90;
    enemy[2].enemy_type = 1; // shadow1 reused
}

void redraw_combat_screen()
{
    uart_puts("[REDRAW_COMBAT_UI] Redrawing combat screen...\n");

    // Draw background
    drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

    // Redraw protagonists with correct sprites and HP bars
    CharacterSprite sprites[MAX_PROTAGONISTS] = {
        {&protagonists[0], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 452, 500},
        {&protagonists[1], char1, PROTAG_WIDTH, PROTAG_HEIGHT, 280, 330},
        {&protagonists[2], char2, 80, 88, 664, 300},
        {&protagonists[3], char3, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}
    };

    for (int i = 0; i < MAX_PROTAGONISTS; ++i) {
        draw_character_sprite(&sprites[i]);
    }

    // Redraw enemies with correct sprites and HP bars
    EnemySprite enemies_sprites[MAX_ENEMIES] = {
        {&enemy[0], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 370, 320 },
        {&enemy[1], shadow2, ENEMY2_WIDTH, ENEMY2_HEIGHT, 445, 370 },
        {&enemy[2], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 520, 320 }
    };

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        draw_enemy_sprite(&enemies_sprites[i]);
    }

    // Redraw buttons
    draw_attack_button(0);
    draw_item_button(0);
    draw_persona_button(0);
    draw_run_button(0);
    draw_skill_button(0);

    // Show updated buffer
    swap_buffers();
}