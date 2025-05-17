// Screen dimensions
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Header
#define COLOR_COMMAND_HEADER 0xFFFFFF00  // Yellow
#define COLOR_STATS_BG       0xFF2894F1  // #2894f1 (Blue)
#define COLOR_STATS_TEXT     0xFFFFFFFF  // White

// Lobby image position and size
#define IMG_START_X 0
#define IMG_START_Y 0
#define LOBBY_WIDTH 1024
#define LOBBY_HEIGHT 768

// Menu button layout
#define MENU_ITEM_COUNT 4
#define BUTTON_WIDTH   100
#define BUTTON_HEIGHT  30
#define BUTTON_SPACING 5
#define BUTTON_START_X 10
#define BUTTON_START_Y 10

#define BUTTON_COLOR     0xFF444444  // Dark gray background
#define HIGHLIGHT_COLOR  0xFF2894F1  // Blue highlight
#define TEXT_COLOR       0xFFFFFFFF  // White text

// Font properties
#define FONT_HEIGHT 8         // Base font height in pixels
#define FONT_SCALE 2          // Scale used in drawString_double_buffering

// Text padding for button label
#define TEXT_PADDING_X 10
#define TEXT_PADDING_Y ((BUTTON_HEIGHT - (FONT_HEIGHT * FONT_SCALE)) / 2)

// Bottom button layout (Enter and Back)
#define BTN_WIDTH   170
#define BTN_HEIGHT  30
#define BTN_MARGIN  20

#define ENTER_BTN_X (SCREEN_WIDTH - BTN_WIDTH - BTN_MARGIN)
#define ENTER_BTN_Y (SCREEN_HEIGHT - BTN_HEIGHT - BTN_MARGIN)

#define BACK_BTN_X  (ENTER_BTN_X - BTN_WIDTH - BTN_MARGIN)
#define BACK_BTN_Y  ENTER_BTN_Y

#define ENTER_BTN_LABEL "Enter"
#define BACK_BTN_LABEL  "Back(ESC)"

// Arrow key codes (via escape sequence: ESC [ A / B)
#define ESC_MENU    0x1B

#define ENTER_KEY 0x0D  // ASCII Carriage Return

// Function declarations
void displayLobbyScreen();
void drawRectARGB32_double_buffering_menu(int x, int y, int width, int height, unsigned int attr, int fill);
void draw_menu_buttons();
void lobby_screen_loop();
