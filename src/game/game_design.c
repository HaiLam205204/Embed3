#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/game_design.h"
#include "../../include/game_map.h"
#include "../../include/protagonist_sprite.h"

#define PLACEHOLDER_COLOR 0xFFFFFFFF // White placeholder color
#define MAX_PLACEHOLDERS 4

int placeholder_positions[MAX_PLACEHOLDERS][2] = {
    {100, 100},
    {200, 100},
    {300, 100},
    {400, 100},
};

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
                    int x = placeholder_positions[i][0];
                    int y = placeholder_positions[i][1];

                    drawImage_double_buffering(PROTAG_START_X, PROTAG_START_X, myBitmapprotag, PROTAG_WIDTH, PROTAG_HEIGHT);
                    uart_puts("[DESIGN_SCREEN] Drawn placeholder at (");
                    uart_dec(x);
                    uart_puts(", ");
                    uart_dec(y);
                    uart_puts(")\n");
                }

                swap_buffers();
            }
            first_frame = !first_frame;
        }
    }
    uart_puts("[DESIGN_SCREEN] Design Screen Render Complete.\n");
}