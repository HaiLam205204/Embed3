#include "../../include/lobbyScreen.h"
#include "../../include/framebf.h"
#include "../../include/uart0.h"
#include "../../include/gpio.h"
#include "../../include/utils.h"
#include "../../include/game_menu.h"
#include "../../include/protagonist_menu.h"
int selected_menu_index = 0;  // Start with the first button selected
const char* menu_labels[MENU_ITEM_COUNT] = {"Skill", "Item", "Persona", "System"};

void drawRectARGB32_double_buffering(int x, int y, int width, int height, unsigned int attr, int fill)
{
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (fill || i == 0 || j == 0 || i == width - 1 || j == height - 1) {
                drawPixelARGB32_double_buffering(x + i, y + j, attr);
            }
        }
    }
}
void draw_menu_buttons() {
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        int y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * i;

        if (i == selected_menu_index) {
            drawRectARGB32_double_buffering(BUTTON_START_X, y, BUTTON_WIDTH, BUTTON_HEIGHT, HIGHLIGHT_COLOR, 1);
        }

        drawString_double_buffering(
            BUTTON_START_X + TEXT_PADDING_X,
            y + TEXT_PADDING_Y,
            (char *)menu_labels[i],
            TEXT_COLOR,
            FONT_SCALE
        );
    }
}
void draw_bottom_buttons() {
    // Draw Back button
    drawRectARGB32_double_buffering(BACK_BTN_X, BACK_BTN_Y, BTN_WIDTH, BTN_HEIGHT, BUTTON_COLOR, 2);
    drawString_double_buffering(BACK_BTN_X + 20, BACK_BTN_Y + 8, BACK_BTN_LABEL, TEXT_COLOR, 2);

    // Draw Enter button
    drawRectARGB32_double_buffering(ENTER_BTN_X, ENTER_BTN_Y, BTN_WIDTH, BTN_HEIGHT, BUTTON_COLOR, 1);
    drawString_double_buffering(ENTER_BTN_X + 20, ENTER_BTN_Y + 8, ENTER_BTN_LABEL, TEXT_COLOR, 2);
}
void draw_command_header() {
    const char* header = "Command";
    int header_width = 8 * strlen(header) * 2;  // Font scale 2
    int x = (SCREEN_WIDTH - header_width) / 2;
    drawString_double_buffering(x, 20, (char*)header, COLOR_COMMAND_HEADER, 2);
}
void draw_stats_panel() {
    const char* name = "Pontagolist";
    const char* level = "Level: 8";
    const char* hp = "HP: 72 / 100";
    const char* mp = "MP: 30 / 50";

    int panel_x = BUTTON_START_X;
    
    // Move panel further down (e.g., 80px after last button)
    int panel_y = BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_SPACING) * MENU_ITEM_COUNT + 80;

    int panel_width = 320;
    int panel_height = 120;

    // Draw blue background
    drawRectARGB32_double_buffering(panel_x, panel_y, panel_width, panel_height, COLOR_STATS_BG, 1);

    // Increase font size to 2
    drawString_double_buffering(panel_x + 10, panel_y + 10, (char*)name, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 40, (char*)level, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 70, (char*)hp, COLOR_STATS_TEXT, 2);
    drawString_double_buffering(panel_x + 10, panel_y + 100, (char*)mp, COLOR_STATS_TEXT, 2);
}

void displayLobbyScreen(){

    clear_screen(0xFFFF0000);
    drawImage_double_buffering(IMG_START_X, IMG_START_Y, lobbyScreen, LOBBY_WIDTH,LOBBY_HEIGHT);
    draw_command_header();
    draw_menu_buttons();
    draw_bottom_buttons();
    draw_stats_panel();
}

void lobby_screen_loop() {
    while (1) {
        for (int i = 0; i < 2; i++) {
            displayLobbyScreen(); // Redraw menu
            swap_buffers(); 
            wait_us(16000); 
        }

        char key = uart_getc();

        if (key == 'w' || key == 'W') {
            uart_puts("W key pressed (UP)\n");
            selected_menu_index = (selected_menu_index - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
        } else if (key == 's' || key == 'S') {
            uart_puts("S key pressed (DOWN)\n");
            selected_menu_index = (selected_menu_index + 1) % MENU_ITEM_COUNT;
        } else if (key == ESC_MENU) { // ESC to exit
            break;
        }
    }
} 