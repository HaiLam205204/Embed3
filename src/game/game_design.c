#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/game_design.h"
#include "../../include/bitmaps/game_map.h"
#include "../../include/bitmaps/protagonist_sprite.h"
#include "../../include/game_combat.h"
#include "../../include/combat_character.h"
#include "../../include/models/character_sprite.h"

#define MAX_PLACEHOLDERS 4

int placeholder_positions[MAX_PLACEHOLDERS][2] = {
    {452, 500},
    {280, 330},
    {664, 300},
    {472, 150},
};

void design_screen_loop()
{
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;

    Character protagonists[MAX_PLACEHOLDERS] = {
        {"Hero", 1, 100, 100},
        {"Ally1", 0, 80, 100},
        {"Ally2", 0, 50, 100},
        {"Ally3", 0, 20, 100}
    };

    CharacterSprite sprites[MAX_PLACEHOLDERS] = {
        {&protagonists[0], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 452, 500},
        {&protagonists[1], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 280, 330},
        {&protagonists[2], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 664, 300},
        {&protagonists[3], myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT, 472, 150}
    };

    while (1)
    {
        if (first_frame)
        {
            uart_puts("[FRAME] Draw background");
            for (int i = 0; i < 2; i++)
            {
                drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

                for (int i = 0; i < MAX_PLACEHOLDERS; ++i) {
                    draw_character_sprite(&sprites[i]);
                }

                swap_buffers();
            }
            first_frame = !first_frame;
        }

        combat_utility_UI();
    }

    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}

void redraw_combat_screen() {
    uart_puts("[REDRAW_COMBAT_UI] Redrawing combat screen...\n");
    // Redraw map background
    drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

    // Redraw all placeholder characters
    for (int i = 0; i < MAX_PLACEHOLDERS; ++i) {
        int pos_x = placeholder_positions[i][0];
        int pos_y = placeholder_positions[i][1];
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