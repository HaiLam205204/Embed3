#include "../header/color.h"

int change_color(char *text, char *background)
{
    const char *colors[] = {"BLACK", "RED", "YELLOW", "BLUE", "PURPLE", "CYAN", "WHITE"};

    // Match text color
    for (int i = 0; i < sizeof(colors) / sizeof(colors[0]); i++)
    {
        if (compare_string(text, colors[i]) == 0)
        {
            switch (i)
            {
            case 0:
                uart_puts(BLACK_TEXT);
                break;
            case 1:
                uart_puts(RED_TEXT);
                break;
            case 2:
                uart_puts(YELLOW_TEXT);
                break;
            case 3:
                uart_puts(BLUE_TEXT);
                break;
            case 4:
                uart_puts(PURPLE_TEXT);
                break;
            case 5:
                uart_puts(CYAN_TEXT);
                break;
            case 6:
                uart_puts(WHITE_TEXT);
                break;
            default:
                break;
            }
            break;
        }
    }

    // Match background color
    for (int i = 0; i < sizeof(colors) / sizeof(colors[0]); i++)
    {
        if (compare_string(background, colors[i]) == 0)
        {
            switch (i)
            {
            case 0:
                uart_puts(BLACK_BACKGROUND);
                break;
            case 1:
                uart_puts(RED_BACKGROUND);
                break;
            case 2:
                uart_puts(YELLOW_BACKGROUND);
                break;
            case 3:
                uart_puts(BLUE_BACKGROUND);
                break;
            case 4:
                uart_puts(PURPLE_BACKGROUND);
                break;
            case 5:
                uart_puts(CYAN_BACKGROUND);
                break;
            case 6:
                uart_puts(WHITE_BACKGROUND);
                break;
            default:
                break;
            }
            break;
        }
    }

    return 0;
}
