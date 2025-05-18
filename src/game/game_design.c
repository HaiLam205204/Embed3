#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/game_design.h"
#include "../../include/bitmaps/game_map.h"
#include "../../include/bitmaps/protagonist_sprite.h"
#include "../../include/bitmaps/character1.h"
#include "../../include/bitmaps/character2.h"
#include "../../include/bitmaps/character3.h"
#include "../../include/game_combat.h"
#include "../../include/combat_character.h"
#include "../../include/models/character.h"
#include "../../include/models/character_sprite.h"

#define MAX_PLACEHOLDERS 4

int positions[MAX_PLACEHOLDERS][2] = {
    {452, 500},
    {280, 330},
    {664, 300},
    {472, 150},
};

Character protagonists[MAX_PLACEHOLDERS];

// Function prototypes
void init_protagonists();
void take_turn(int character_index);

void design_screen_loop()
{
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;
    int turn_index = 0; // Track whose turn it is

    init_protagonists();

    CharacterSprite sprites[MAX_PLACEHOLDERS] = {
        {&protagonists[0], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 452, 500},
        {&protagonists[1], char1, PROTAG_WIDTH, PROTAG_HEIGHT, 280, 330},
        {&protagonists[2], char2, 80, 88, 664, 300},
        {&protagonists[3], char3, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}};

    while (1)
    {
        if (first_frame)
        {
            uart_puts("[FRAME] Draw background\n");
            for (int i = 0; i < 2; i++)
            {
                drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

                for (int i = 0; i < MAX_PLACEHOLDERS; ++i)
                {
                    draw_character_sprite(&sprites[i]);
                }

                swap_buffers();
            }
            first_frame = 0;
        }

        // Simple turn iteration
        uart_puts("[TURN] It's ");
        uart_puts(protagonists[turn_index].name);
        uart_puts("'s turn!\n");

        take_turn(turn_index);

        // Move to the next character, looping around
        turn_index = (turn_index + 1) % MAX_PLACEHOLDERS;

        // Wait for input to continue to next turn (simple simulation)
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

void redraw_combat_screen()
{
    uart_puts("[REDRAW_COMBAT_UI] Redrawing combat screen...\n");
    // Redraw map background
    drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

    // Redraw all placeholder characters
    for (int i = 0; i < MAX_PLACEHOLDERS; ++i)
    {
        int pos_x = positions[i][0];
        int pos_y = positions[i][1];
        drawImage_double_buffering(pos_x, pos_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
    }

    // Redraw buttons (off-state by default)
    draw_attack_button(0);
    draw_item_button(0);
    draw_persona_button(0);
    draw_run_button(0);
    draw_skill_button(0);

    // Swap to make the changes visible
    swap_buffers();
}