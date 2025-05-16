#include "../../include/combat_interface.h"
#include "../../include/framebf.h"
#include "../../include/uart0.h"

#define HP_BAR_WIDTH 50
#define HP_BAR_HEIGHT 5
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00

void draw_hp_bar(int x, int y, int hp)
{
    int green_width = (HP_BAR_WIDTH * hp) / 100;
    int red_width = HP_BAR_WIDTH - green_width;

    // Draw green part (current HP) on back buffer
    drawRectARGB32_double_buffering(x, y - 10, x + green_width, y - 10 + HP_BAR_HEIGHT, GREEN, 1);

    // Draw red part (lost HP) on back buffer
    if (red_width > 0)
    {
        drawRectARGB32_double_buffering(x + green_width, y - 10, x + green_width + red_width, y - 10 + HP_BAR_HEIGHT, RED, 1);
    }

    uart_puts("[COMBAT_SCREEN] Drawn hp bar: ");
    uart_dec(hp);
    uart_puts("\n");
}

void draw_combat_character(int pos_x, int pos_y, const unsigned long *character_bitmap, int image_width, int image_height)
{
    drawImage_double_buffering(pos_x, pos_y, character_bitmap, image_width, image_height);
    uart_puts("[COMBAT_SCREEN] Drawn character at (");
    uart_dec(pos_x);
    uart_puts(", ");
    uart_dec(pos_y);
    uart_puts(")\n");
}