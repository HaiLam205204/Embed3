#include "../../../include/bitmaps/lobbyScreen.h"
#include "../../../include/framebf.h"
#include "../../../include/uart0.h"
#include "../../../include/gpio.h"
#include "../../../include/utils.h"
#include "../../../include/game_menu.h"
#include "../../../include/bitmaps/protagonist_menu.h"
#include "../../../include/models/character.h"
#include "../../../include/game_combat.h"
int selected_menu_index = 0; // Start with the first button selected
const char *menu_labels[MENU_ITEM_COUNT] = {"Skill", "Item", "Persona", "System"};
int is_persona = 0; 
int is_Item = 0; 
int is_skill= 0; 
int is_System = 0; 

void drawRectARGB32_double_buffering_menu(int x, int y, int width, int height, unsigned int attr, int fill)
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
void draw_menu_buttons()
{
    for (int i = 0; i < MENU_ITEM_COUNT; i++)
    {
        int y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * i;

        if (i == selected_menu_index)
        {
            drawRectARGB32_double_buffering_menu(BUTTON_START_X, y, BUTTON_WIDTH, BUTTON_HEIGHT, HIGHLIGHT_COLOR, 1);
        }

        drawString_double_buffering(
            BUTTON_START_X + TEXT_PADDING_X,
            y + TEXT_PADDING_Y,
            (char *)menu_labels[i],
            TEXT_COLOR,
            FONT_SCALE);
    }
}
void draw_bottom_buttons()
{
    // Draw Back button
    drawRectARGB32_double_buffering_menu(BACK_BTN_X, BACK_BTN_Y, BTN_WIDTH, BTN_HEIGHT, BUTTON_COLOR, 2);
    drawString_double_buffering(BACK_BTN_X + 20, BACK_BTN_Y + 8, BACK_BTN_LABEL, TEXT_COLOR, 2);

    // Draw Enter button
    drawRectARGB32_double_buffering_menu(ENTER_BTN_X, ENTER_BTN_Y, BTN_WIDTH, BTN_HEIGHT, BUTTON_COLOR, 1);
    drawString_double_buffering(ENTER_BTN_X + 20, ENTER_BTN_Y + 8, ENTER_BTN_LABEL, TEXT_COLOR, 2);
}
void draw_command_header()
{
    const char *header = "Command";
    int header_width = 8 * strlen(header) * 2; // Font scale 2
    int x = (SCREEN_WIDTH - header_width) / 2;
    drawString_double_buffering(x, 20, (char *)header, COLOR_COMMAND_HEADER, 2);
}
void int_to_string(int num, char *str)
{
    int i = 0, j, digit;
    char temp[10];

    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0)
    {
        digit = num % 10;
        temp[i++] = digit + '0';
        num /= 10;
    }

    // Reverse string
    for (j = 0; j < i; j++)
    {
        str[j] = temp[i - j - 1];
    }

    str[j] = '\0';
}

void draw_stats_panel(Character *ch)
{
    const char *name = "Pontagolist";
    const char *level = "Level: 0";

    // Manually build "HP: current_hp / max_hp"
    char hp[32];
    char current_hp_str[10];
    char max_hp_str[10];

    // Convert integers to strings
    int_to_string(ch->current_hp, current_hp_str);
    int_to_string(ch->max_hp, max_hp_str);

    // Build hp string: "HP: " + current_hp_str + " / " + max_hp_str
    int i = 0;
    hp[i++] = 'H';
    hp[i++] = 'P';
    hp[i++] = ':';
    hp[i++] = ' ';

    int j = 0;
    while (current_hp_str[j] != '\0')
    {
        hp[i++] = current_hp_str[j++];
    }

    hp[i++] = ' ';
    hp[i++] = '/';
    hp[i++] = ' ';

    j = 0;
    while (max_hp_str[j] != '\0')
    {
        hp[i++] = max_hp_str[j++];
    }

    hp[i] = '\0'; // null-terminate

    const char *mp = "MP: 30 / 50";

    int panel_x = BUTTON_START_X;
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    drawRectARGB32_double_buffering_menu(panel_x, panel_y, panel_width, panel_height, COLOR_STATS_BG, 1);
    drawString_double_buffering(panel_x + 10, panel_y + 10, (char *)name, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 40, (char *)level, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 70, hp, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 100, (char *)mp, COLOR_STATS_TEXT, 2);
}
void draw_item_panel()
{
    const char *line1 = "ITEM";
    const char *line2 = "Function: healing 25% the maximun HP";
    const char *line3 = "Objective: Only for the main Character";

    int panel_x = BUTTON_START_X;
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    drawRectARGB32_double_buffering_menu(panel_x + 20 + panel_width , panel_y, panel_width * 2, panel_height, COLOR_STATS_BG, 1);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 10, (char *)line1, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 40, (char *)line2, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 70, (char *)line3, COLOR_STATS_TEXT, 2);
}
void draw_skill_panel()
{
    const char *line1 = "Skills: ";
    const char *line2 = "Skill 1: attack single object";
    const char *line3 = "Skill 2: attack single multiple objects";

    int panel_x = BUTTON_START_X;
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    drawRectARGB32_double_buffering_menu(panel_x + 20 + panel_width , panel_y, panel_width * 2, panel_height, COLOR_STATS_BG, 1);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 10, (char *)line1, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 40, (char *)line2, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 70, (char *)line3, COLOR_STATS_TEXT, 2);
}
void draw_persona_panel()
{
    const char *line1 = "Persona";
    const char *line2 = "Persona 1: Orpheus";
    const char *line3 = "Persona 2: Pixie";

    int panel_x = BUTTON_START_X;
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    drawRectARGB32_double_buffering_menu(panel_x + 20 + panel_width , panel_y, panel_width, panel_height, COLOR_STATS_BG, 1);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 10, (char *)line1, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 40, (char *)line2, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 70, (char *)line3, COLOR_STATS_TEXT, 2);
}
void draw_system_panel()
{
    const char *line1 = "System";
    const char *line2 = "Raspberry Pi board";
    const char *line3 = "Embedded System: Operating and Interaction";

    int panel_x = BUTTON_START_X;
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    drawRectARGB32_double_buffering_menu(panel_x + 20 + panel_width , panel_y, panel_width * 2, panel_height, COLOR_STATS_BG, 1);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 10, (char *)line1, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 40, (char *)line2, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10+ 20 + panel_width, panel_y + 70, (char *)line3, COLOR_STATS_TEXT, 2);
}
void setPotangonist(Character *ch)
{
    strcpy(ch->name, "Hero");
    ch->is_main_character = 1;
    ch->current_hp = 100;
    ch->max_hp = 100;
    ch->healing_item_quantity = 1;
}
void displayLobbyScreen()
{
    // Set protagonist and draw main lobby background
    setPotangonist(&protagonists[0]);
    drawImage_double_buffering(IMG_START_X, IMG_START_Y, lobbyScreen, LOBBY_WIDTH, LOBBY_HEIGHT);

    // Draw UI components
    draw_command_header();
    draw_menu_buttons();
    draw_bottom_buttons();
    draw_stats_panel(&protagonists[0]);

    // Conditionally overlay skill/persona panels
    if (is_Item)
    {
        draw_item_panel(); // You can overlay this somewhere visible
    }

    if (is_persona)
    {
        draw_persona_panel();
         // Pass value to draw persona screen
    }
        if (is_skill)
    {
        draw_skill_panel();
         // Pass value to draw persona screen
    }
        if (is_System)
    {
        draw_system_panel();
         // Pass value to draw persona screen
    }

    // You can add more like:
    // if (is_Item) draw_item_panel();
    // if (is_System) draw_system_panel();
}


void lobby_screen_loop() 
{
    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            displayLobbyScreen(); // Redraw menu with selected_menu_index
            swap_buffers();
            wait_us(16000); // Roughly 60 FPS
        }

        char key = uart_getc();

        if (key == 'w' || key == 'W')
        {
            uart_puts("W key pressed (UP)\n");
            selected_menu_index = (selected_menu_index - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
        }
        else if (key == 's' || key == 'S')
        {
            uart_puts("S key pressed (DOWN)\n");
            selected_menu_index = (selected_menu_index + 1) % MENU_ITEM_COUNT;
        }
        else if (key == '\r' || key == '\n') // Enter key
        {
            uart_puts("Enter key pressed\n");

            // Reset all flags
            is_persona = is_Item = is_skill = is_System = 0;

            // Activate the selected menu
            switch (selected_menu_index)
            {
                case 0:
                    is_skill = 1;
                    uart_puts("Skill selected!\n");
                    break;
                case 1:
                    is_Item = 1;
                    uart_puts("Item selected!\n");
                    break;
                case 2:
                    is_persona = 1;
                    uart_puts("Persona selected!\n");
                    break;
                case 3:
                    is_System = 1;
                    uart_puts("System selected!\n");
                    break;
            }

            // You can now call a function depending on the flag, for example:
            // if (is_skill) showSkillMenu();
            // etc.
        }
        else if (key == ESC_MENU) // ESC to exit
        {
            break;
        }
    }
}