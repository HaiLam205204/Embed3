#include "gpio.h"

#define PROMPT "MyOS> "
#define MAX_BUFFER 128 // Input buffer size
#define NULL ((void *)0) // pointer that doesn't point to any valid memory location

// CLI colors (ARGB format)
#define BLUE   0x000000CC 
#define WHITE 0xFFFFFFFF 
#define BLACK 0xFF000000 

// Zoom
#define ZOOM 2

// font size
#define FONT_WIDTH 8
#define FONT_HEIGHT 8

// CLI border positions
#define CLI_LEFT   100
#define CLI_TOP    100
#define CLI_RIGHT  700
#define CLI_BOTTOM 500

// CLI window size
#define CLI_COLS ((CLI_RIGHT - CLI_LEFT) / FONT_WIDTH)
#define CLI_ROWS ((CLI_BOTTOM - CLI_TOP) / FONT_HEIGHT)

// History 
#define MAX_HISTORY 20

// Auto-complete
#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

// Define the ASCII code for Escape key
#define ESC 27  


void draw_cli_window();
void cli_put_char(char ch, unsigned int attr, int zoom);
void cli_put_string(const char* str, unsigned int attr, int zoom);
void cli_put_hex(uint32_t num, unsigned int attr, int zoom);
void cli_put_hex8(uint8_t byte, unsigned int attr, int zoom);
void clear_cli_window();
void cli_loop();
char* get_prev_command(char* input_buffer);
char* get_next_command(void);
void add_to_history(char* command);
int find_matches(char* partial, char* matches[], int max_matches);
void autocomplete(char* buffer, int* index);
void handle_command(char *command);
void show_all_help();
void show_command_help(char* command_name);

