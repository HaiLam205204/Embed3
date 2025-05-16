#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/game_design.h"
#include "../../include/game_map.h"
#include "../../include/protagonist_sprite.h"
#include "../../include/combat_interface.h"

#define MAX_PLACEHOLDERS 4

int placeholder_positions[MAX_PLACEHOLDERS][2] = {
    {452, 500},
    {280, 330},
    {664, 300},
    {472, 150},
};

int character_hp[MAX_PLACEHOLDERS] = {100, 80, 50, 20};

void design_screen_loop()
{
    uart_puts("[DESIGN_SCREEN] Entering Design Screen...\n");
    static int first_frame = 1;
    while (1)
    {
        if (first_frame)
        {
            uart_puts("[FRAME] Draw background");
            for (int i = 0; i < 2; i++)
            {
                drawImage_double_buffering(MAP_START_X, MAP_START_Y, game_map, GAME_MAP_WIDTH, GAME_MAP_HEIGHT);

                for (int i = 0; i < MAX_PLACEHOLDERS; ++i)
                {
                    int pos_x = placeholder_positions[i][0];
                    int pos_y = placeholder_positions[i][1];

                    draw_combat_character(pos_x, pos_y, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
                }

                for (int i = 0; i < MAX_PLACEHOLDERS; ++i)
                {
                    int pos_x = placeholder_positions[i][0];
                    int pos_y = placeholder_positions[i][1];
                    draw_hp_bar(pos_x, pos_y, character_hp[i]);
                }

                swap_buffers();
            }
            first_frame = !first_frame;
        }
    }
    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}