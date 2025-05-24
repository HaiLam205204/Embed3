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
// #include "../../include/bitmaps/yellow_triangle.h"
#include "../../include/game_logic.h"
#include "../../include/game_menu.h"
#include "../../include/utils.h"

// #define MAX_PROTAGONISTS 4
// #define MAX_ENEMIES 3
#define BTN_WIDTH_REWARD 150
#define BTN_HEIGHT_REWARD 40
#define BTN_SELECTED_COLOR 0xFF00509E // Dark blue
#define BTN_NORMAL_COLOR 0xFF2894F1   // Light blue
#define TEXT_ZOOM 2
extern int current_player_turn;

int positions[MAX_PROTAGONISTS][2] = {
    {452, 500},
    {280, 330},
    {664, 300},
    {472, 150},
};

int enemy_positions[MAX_ENEMIES][2] = {
    {350, 320}, // Enemy 1
    {400, 370}, // Enemy 2
    {450, 320}  // Enemy 3
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
    {&protagonists[3], char3, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}};

EnemySprite enemy_sprites[MAX_ENEMIES] = {
    {&enemy[0], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 370, 270},
    {&enemy[1], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 475, 360},
    {&enemy[2], shadow1, ENEMY1_WIDTH, ENEMY1_HEIGHT, 520, 270},
};

void design_screen_loop()
{
    char input;
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;

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
                    draw_turn_indicator(&sprites[current_player_turn], 466, 593);
                }

                for (int i = 0; i < num_enemies; ++i)
                {
                    draw_enemy_sprite(&enemy_sprites[i]);
                }

                swap_buffers();
            }
            first_frame = 0;
        }

        input = uart_getc();

        if (input == 'm' || input == 'M')
        {
            uart_puts("\n[INPUT] 'M' pressed - Switching to Menu Screen...\n");
            lobby_screen_loop();
            first_frame = 1; // Redraw on return
            continue;
        }

        combat_utility_UI(protagonists, num_protagonists, enemy, num_enemies);
    }

    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}

void init_protagonists()
{
    strcpy(protagonists[0].name, "Hero");
    protagonists[0].is_main_character = 1;
    protagonists[0].current_hp = 50;
    protagonists[0].max_hp = 100;
    protagonists[0].healing_item_quantity = 1;

    strcpy(protagonists[1].name, "Ally1");
    protagonists[1].is_main_character = 0;
    protagonists[1].current_hp = 100;
    protagonists[1].max_hp = 100;

    strcpy(protagonists[2].name, "Ally2");
    protagonists[2].is_main_character = 0;
    protagonists[2].current_hp = 1;
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

    for (int i = 0; i < num_protagonists; ++i)
    {
        draw_character_sprite(&sprites[i]);
    }

    for (int i = 0; i < num_enemies; ++i)
    {
        draw_enemy_sprite(&enemy_sprites[i]);
    }

    // Redraw buttons
    draw_attack_button(0);
    draw_item_button(0, &protagonists[0]);
    draw_persona_button(0);
    draw_run_button(0);
    draw_skill_button(0);

    // === Yellow triangle turn indicator ===
    CharacterSprite *current_sprite = &sprites[current_player_turn];

    int triangle_x = current_sprite->pos_x + (current_sprite->width / 2) - (40 / 2);
    int triangle_y = current_sprite->pos_y + current_sprite->height + 5; // Just below the sprite

    draw_turn_indicator(current_sprite, triangle_x, triangle_y);
    // === Highlight selected enemy if in selection screen ===
    if (current_screen == SCREEN_SELECT_ENEMY)
    {
        EnemySprite *selected_enemy = &enemy_sprites[selected_enemy_index];
        int enemy_triangle_x = selected_enemy->pos_x + (selected_enemy->width / 2) - (40 / 2);
        int enemy_triangle_y = selected_enemy->pos_y + selected_enemy->height + 5;
        draw_enemy_selected(selected_enemy, enemy_triangle_x, enemy_triangle_y);
    }
    // Show updated buffer
    swap_buffers();
}
void heal_character_25_percent(Character *ch)
{
    if (!ch)
        return;
    if (ch->healing_item_quantity < 1)
    {
        uart_puts("[HEAL] No healing items left.\n");
        return;
    }

    if (ch->current_hp >= ch->max_hp)
    {
        uart_puts("[HEAL] ");
        uart_puts(ch->name);
        uart_puts("'s HP is already full. Cannot heal.\n");
        return;
    }

    int original_hp = ch->current_hp;
    int heal_amount = ch->max_hp / 4;

    ch->current_hp += heal_amount;
    if (ch->current_hp > ch->max_hp)
    {
        ch->current_hp = ch->max_hp;
    }

    ch->healing_item_quantity--;

    int actual_healed = ch->current_hp - original_hp;

    uart_puts("[HEAL] ");
    uart_puts(ch->name);
    uart_puts(" used a healing item.\n");
    uart_puts("Healed for ");
    uart_dec(actual_healed);
    uart_puts(" HP. Current HP: ");
    uart_dec(ch->current_hp);
    uart_puts("/");
    uart_dec(ch->max_hp);
    uart_puts("\nRemaining healing items: ");
    uart_dec(ch->healing_item_quantity);
    uart_puts("\n");
    // draw_character_sprite(&sprites[0]);
    int hp_percent = (sprites[0].character->current_hp * 100) / sprites[0].character->max_hp;
    draw_hp_bar(sprites[0].pos_x, sprites[0].pos_y, hp_percent);
}

void drawRectARGB32_double_buffering_rewardScreen(int x, int y, int width, int height, unsigned int attr, int fill)
{
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            if (fill || i == 0 || j == 0 || i == width - 1 || j == height - 1)
            {
                drawPixelARGB32_double_buffering(x + i, y + j, attr);
            }
        }
    }
}
void drawRewardPanel(int selected_index)
{
    int rect_width = 700;
    int rect_height = 250;
    int screen_width = 1024;
    int screen_height = 768;

    int panel_x = (screen_width - rect_width) / 2;
    int panel_y = (screen_height - rect_height) / 2;

    // Background panel
    drawRectARGB32_double_buffering_rewardScreen(panel_x, panel_y, rect_width, rect_height, 0xFF2894F1, 1);

    // Title
    char *title = "Reward";
    int title_x = panel_x + (rect_width - 80) / 2;
    int title_y = panel_y + 10;
    drawString_double_buffering(title_x, title_y, title, TEXT_COLOR, TEXT_ZOOM);

    // Button positions
    char *left_btn = "Collect Item";
    char *right_btn = "New Persona";
    char *bottom_btn = "Enter";

    int left_btn_x = panel_x + 30;
    int left_btn_y = panel_y + 100;

    int right_btn_x = panel_x + rect_width - 180;
    int right_btn_y = panel_y + 100;

    int bottom_btn_y = panel_y + rect_height - 40;

    // Draw button backgrounds
    drawRectARGB32_double_buffering_rewardScreen(
        left_btn_x - 10, left_btn_y - 10, BTN_WIDTH_REWARD + 50, BTN_HEIGHT_REWARD,
        selected_index == 0 ? BTN_SELECTED_COLOR : BTN_NORMAL_COLOR, 1);
    drawRectARGB32_double_buffering_rewardScreen(
        right_btn_x - 10, right_btn_y - 10, BTN_WIDTH_REWARD + 20, BTN_HEIGHT_REWARD,
        selected_index == 1 ? BTN_SELECTED_COLOR : BTN_NORMAL_COLOR, 1);

    // Draw text
    drawString_double_buffering(left_btn_x, left_btn_y, left_btn, TEXT_COLOR, TEXT_ZOOM);
    drawString_double_buffering(right_btn_x, right_btn_y, right_btn, TEXT_COLOR, TEXT_ZOOM);
    drawString_double_buffering(700, bottom_btn_y + 10, bottom_btn, TEXT_COLOR, TEXT_ZOOM);
}
void clear_screen_buffer()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            drawPixelARGB32_double_buffering(x, y, 0xFF000000); // black
        }
    }
}
void displayRewardScreen()
{
    int selected = 0;

    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            drawRewardPanel(selected); // draw based on current selection
            swap_buffers();
            wait_us(16000);
        }

        char ch = getUart();

        if (ch == 'a' || ch == 'A')
        {
            selected = 0;
        }
        else if (ch == 'd' || ch == 'D')
        {
            selected = 1;
        }
        else if (ch == '\r' || ch == '\n')
        {
            // Do something based on selection
            if (selected == 0)
            { // Collect Item
                protagonists[0].healing_item_quantity += 1;
                uart_puts("Collected item! Healing item quantity increased.\n");
            }
            else if (selected == 1)
            { // New Persona
                
                uart_puts("New Persona selected. (Not implemented yet)\n");
            }

            // Exit the reward panel
            break;
        }
    }
}
