// Prompt: MyOS>
// Buffer user input.
// Recognize special keys like \n, _, +, \t, and backspace.
// Handle enter to parse and execute.

#include "../../include/CLI.h"
#include "../../include/utils.h"
#include "../../include/uart0.h"
#include "../../include/framebf.h"
#include "../../include/mbox.h"
#include "../../include/bitmaps/welcomeScreen.h"
#include "../../include/renderFrame.h"
#include "../../include/bitmaps/video.h"
#include "../../include/game.h"

// CLI cursor position
int cursorX = CLI_LEFT + 1;
int cursorY = CLI_TOP + 1;

// Command history 
char command_history[MAX_HISTORY][MAX_BUFFER];
int history_index = -1; // -1 means not browsing history
char current_input[MAX_BUFFER]; // stores current input when browsing history
int history_start = 0;  // Index of oldest command
int history_count = 0;  // Number of commands stored

// Function to initialize CLI window
void draw_cli_window() {
    // Draw the background for the CLI window (popup)
    drawRectARGB32(CLI_LEFT, CLI_TOP, CLI_RIGHT, CLI_BOTTOM, BLUE, 1); 
}

// prints characters inside CLI region
void cli_put_char(char ch, unsigned int attr, int zoom) {
    // Static cursor position (relative to CLI window)
    // Initialized just inside the CLI box (1px padding)
    // static int cursorX = CLI_LEFT + 1;
    // static int cursorY = CLI_TOP + 1;

    // Compute the maximum valid coordinates before wrapping or scrolling
    int maxX = CLI_RIGHT - (FONT_WIDTH * zoom); // 1 screen character before the edge
    int maxY = CLI_BOTTOM - (FONT_HEIGHT * zoom);

    // Handle special characters
    if (ch == '\r') {
        // Carriage return: go to beginning of current line
        cursorX = CLI_LEFT + 1;
    } else if (ch == '\n') {
        // Newline: reset X and go to next line
        cursorX = CLI_LEFT + 1;
        cursorY += FONT_HEIGHT * zoom;
    } else if (ch == '\b') {
        // Backspace: move cursor left and erase character
        if (cursorX > CLI_LEFT + 1) {
            // Move back one character width
            cursorX -= FONT_WIDTH * zoom;
           
            // Overwrite the character with background color to "erase" it
            drawRectARGB32(
                cursorX,
                cursorY,
                cursorX + (FONT_WIDTH * zoom) - 1,
                cursorY + (FONT_HEIGHT * zoom) - 1,
                BLUE, // background color
                1                 // fill the rectangle
            );
        }
    } else {
        // Regular printable character:
        // Draw the character at current cursor position
        drawChar((unsigned char)ch, cursorX, cursorY, attr, zoom);

        // Move the cursor to the right for next character
        cursorX += FONT_WIDTH * zoom;
    }

    // Wrap text to next line if cursor reaches right window edge
    if (cursorX > maxX) {
        cursorX = CLI_LEFT + 1;
        cursorY += FONT_HEIGHT * zoom;
    }

    // If cursor goes beyond the bottom of the CLI box:
    // We can either scroll or clear and reset — here we reset to top
    if (cursorY > maxY) {
        cursorY = CLI_TOP + 1;

        // Clear entire CLI window area (excluding border)
        drawRectARGB32(
            CLI_LEFT + 1,
            CLI_TOP + 1,
            CLI_RIGHT - 1,
            CLI_BOTTOM - 1,
            BLUE,
            1 // fill
        );
    }
}

// Print a C string to CLI
void cli_put_string(const char* str, unsigned int attr, int zoom) {
    while (*str) {
        cli_put_char(*str++, attr, zoom);
    }
}

// Print hex value
void cli_put_hex(uint32_t num, unsigned int attr, int zoom) {
    const char hex_chars[] = "0123456789ABCDEF";

    cli_put_string("0x", attr, zoom);
    for (int i = 28; i >= 0; i -= 4) {
        char digit = hex_chars[(num >> i) & 0xF];
        cli_put_char(digit, attr, zoom);
    }
}

// Print byte-sized hex (like 0xAB)
void cli_put_hex8(uint8_t byte, unsigned int attr, int zoom) {
    const char hex_chars[] = "0123456789ABCDEF";
    cli_put_char(hex_chars[(byte >> 4) & 0xF], attr, zoom);
    cli_put_char(hex_chars[byte & 0xF], attr, zoom);
}

// Function to handle CLI loop inside the popup window
void cli_loop() {
    // Store input
    char input_buffer[MAX_BUFFER]; 
    int buffer_index = 0;
    // Print prompt
    for (int i = 0; PROMPT[i] != '\0'; i++) { 
        cli_put_char(PROMPT[i], 0x00FF0000, ZOOM);  // Print each character
    }

    while (1) {
        char c = uart_getc();  // Get character input from UART

        // Check exit key FIRST
        if (c == ESC) {
            // Reset cursor position
            cursorX = CLI_LEFT + 1;
            cursorY = CLI_TOP + 1;
            // Draw background image
            draw_background();
            return;              // Exit CLI mode
        }

        if (c == '\r' || c == '\n') { // Enter input
            
            input_buffer[buffer_index] = '\0';  // End of command
            cli_put_char('\n', WHITE, ZOOM); // go to next line
            add_to_history(input_buffer); // add to list of commands in history
            handle_command(input_buffer);  // Process the command
            buffer_index = 0;
            // Print prompt again
            for (int i = 0; PROMPT[i] != '\0'; i++) { 
                cli_put_char(PROMPT[i], 0x00FF0000, ZOOM);  // Print each character
            }
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                cli_put_char('\b', WHITE, ZOOM);
            }
        } else if (c == '_') { // get previous command in the history
            char* cmd = get_prev_command(input_buffer);
            if (cmd) {
                // Clear current line
                while (buffer_index > 0) {
                    cli_put_char('\b', WHITE, ZOOM);
                    buffer_index--;
                }
                strcpy(input_buffer, cmd);
                buffer_index = strlen(cmd);
                cli_put_string(cmd, WHITE, ZOOM);
            }
        } else if (c == '+') { // get next command in the history
            char* cmd = get_next_command();
            if (cmd) {
                // Clear current line
                while (buffer_index > 0) {
                    cli_put_char('\b', WHITE, ZOOM);
                    buffer_index--;
                }
                strcpy(input_buffer, cmd);
                buffer_index = strlen(cmd);
                cli_put_string(cmd, WHITE, ZOOM);
            }
        } else if (c == '\t') { // tab to autocomplete
            autocomplete(input_buffer, &buffer_index);
        } else {
            // If we're typing after recalling a command, mark as modified
            if (history_index != -1) {
                history_index = -1;
            }
            input_buffer[buffer_index++] = c;  // Store character in buffer
            cli_put_char(c, WHITE, ZOOM);  // Display character
        }
    }
}

// ========================
// Command History
// ========================

/**
 * Add command to history if it's not empty or duplicate
 */
void add_to_history(char* command) {
    // Validate input - ignore if:  
    if (command == NULL // Null pointer
        || strlen(command) == 0 // Empty string
        || strlen(command) >= MAX_BUFFER // String too long for our buffer
    ) { 
        return;
    }
    
    // If we're currently browsing history and editing a command
    if (history_index != -1) {
        // Update the current history entry with the edited command
        strcpy(command_history[history_index], command);
        history_index = -1; // Exit history browsing mode
        return;
    }

    // Make sure not adding duplicate consecutive commands
    if (history_count > 0 && string_compare(command_history[(history_start + history_count - 1) % MAX_HISTORY], command) == 0) {
        return;
    }

    // Determine where to store the new command
    int target_index;
    if (history_count < MAX_HISTORY) {
        // Append to end if buffer not full
        target_index = (history_start + history_count) % MAX_HISTORY;
        history_count++;
    } else {
        // Overwrite oldest command (circular behavior)
        target_index = history_start;
        history_start = (history_start + 1) % MAX_HISTORY;
    }

    // Add new command 
    strcpy(command_history[target_index], command);
}

/**
 * Get previous command in history (up arrow key functionality)
 * Returns: Pointer to command string or NULL when no more history available
 * 
 * Behavior:
 * - First call: Saves current input and returns most recent command
 * - Subsequent calls: Returns progressively older commands
 * - Returns NULL when attempting to go beyond oldest command
 * 
 * Circular Buffer Rules:
 * - history_start: Index of oldest command
 * - history_count: Number of valid commands (0 ≤ count ≤ MAX_HISTORY)
 * - history_index: Current navigation position (-1 when not browsing)
 */
char* get_prev_command(char* input_buffer) {
    /* CASE 0: No commands available */
    if (history_count == 0) {
        return NULL;
    }
    
    /* CASE 1: First history access (save current input) */
    if (history_index == -1) {
        strcpy(current_input, input_buffer);
        history_index = (history_start + history_count - 1) % MAX_HISTORY;
        return command_history[history_index];
    }
    
    /* CASE 2: Subsequent access (move backward) */
    int prev_index = (history_index - 1 + MAX_HISTORY) % MAX_HISTORY;
    
    /* Check if we haven't reached the oldest command yet */
    if (prev_index != (history_start - 1 + MAX_HISTORY) % MAX_HISTORY) {
        history_index = prev_index;
        return command_history[history_index];
    }
    
    /* CASE 3: Reached beginning of history */
    return NULL;
}

/**
 * Get next command in history (down arrow key functionality)
 * Returns: Pointer to command string, original input, or NULL
 * 
 * Behavior:
 * - When not browsing history: returns NULL
 * - While browsing: returns progressively newer commands
 * - At end of history: returns original input and resets browsing state
 * 
 * Circular Buffer Rules:
 * - history_start: Index of oldest command
 * - history_count: Number of valid commands
 * - history_index: Current navigation position (-1 when not browsing)
 */
char* get_next_command() {
    /* CASE 0: Not currently browsing history */
    if (history_index == -1) {
        return NULL;
    }
    
    /* Calculate next index with circular buffer wrapping */
    int next_index = (history_index + 1) % MAX_HISTORY;
    
    /* CASE 1: Reached present (after browsing history) */
    if (next_index == (history_start + history_count) % MAX_HISTORY) {
        history_index = -1;  // Reset browsing state

        // // If current_input is empty, return the most recent command instead
        // if (current_input[0] == '\0' && history_count > 0) {
        //     return command_history[(history_start + history_count - 1) % MAX_HISTORY];
        // }
        return current_input;
    }
    
    /* CASE 2: Valid next command in history */
    if ((history_index >= history_start && next_index < history_start + history_count) ||
        (history_start + history_count > MAX_HISTORY && next_index < (history_start + history_count) % MAX_HISTORY)) {
        history_index = next_index;
        return command_history[history_index];
    }
    
    /* CASE 3: Should never reach here if buffer is consistent */
    return NULL;
}

// ========================
// Auto-completion
// ========================

// List of available commands for auto-completion
static const char* commands[] = {
    "help",
    "clear",
    "showinfo",
    "baudrate",
    "handshake",
    "video",
    "game"
};

/**
 * Auto-completes the current command line input based on available commands
 * cli_buffer The input buffer containing the current command line
 * index Pointer to the current cursor position/index in the buffer
 */
void autocomplete(char* cli_buffer, int* index) {
    
    // Early return if buffer is empty
    if (*index == 0) {
        return;
    }
    
    // Initialize to -1 (no space found yet)
    int last_space_index = -1;

    // Scan current input to determine auto-completing a command or not
    for (int i = 0; i < *index; i++) {
        if (cli_buffer[i] == ' ') {
            last_space_index = i;
        }
    }

    // Determine partial input to complete
    char* partial_input = (last_space_index == -1) ? cli_buffer : (cli_buffer + last_space_index + 1);

    // Calculate length of input text 
    int input_len = *index - (last_space_index + 1);

    //Store the matching command
    const char* matched_command = NULL;

    // Count matching commands
    int matched_count = 0;

    // Search for matching commands
    for (int i = 0; i < NUM_COMMANDS; i++) {
        // Check if command starts with our partial input
        if (partial_string_compare(commands[i], partial_input, input_len) == 0) {
            matched_count++;
            // Store matching commands
            if (matched_count == 1) {
                matched_command = commands[i];
            } else { // If multiple matches found
                // Clear current line until nothing is left
                while (*index > 0) { 
                    cli_put_char('\b', WHITE, ZOOM);
                    (*index)--;
                }
                // Print ambiguous message
                cli_put_string("\nAmbiguous command, type more characters.\n", WHITE, ZOOM);
                // Reprint prompt
                for (int i = 0; PROMPT[i] != '\0'; i++) { 
                    cli_put_char(PROMPT[i], 0x00FF0000, ZOOM);
                }
                return;
            }
        }
    }

    // If exactly one match found
    if (matched_count == 1) {
        // Calculate how many characters we need to add
        int match_len = strlen(matched_command);
        int chars_to_add = match_len - input_len; // length of string that need to be auto-completed
        
        if (chars_to_add > 0) {
            // Append the remaining characters to buffer
            for (int i = 0; i < chars_to_add; i++) {
                cli_buffer[*index] = matched_command[input_len + i];
                // Display remaining characters
                cli_put_char(cli_buffer[*index], WHITE, ZOOM); 
                (*index)++;
            }
        }
    }
}

// ========================
// CLI commands
// ========================
void handle_command(char *command) {
    char* cmd = strtok(command, " ");
    char* arg = strtok(NULL, " ");
    if(string_compare(cmd, "help") == 0){
        if(arg == NULL){
            show_all_help();
        } else {
            show_command_help(arg);
        }
    } else if (string_compare(command, "clear") == 0) { // clear the terminal
        // Clear the CLI window contents
        drawRectARGB32(
            CLI_LEFT + 1,
            CLI_TOP + 1,
            CLI_RIGHT - 1,
            CLI_BOTTOM - 1,
            BLUE,
            1
        );
        // Reset cursor position
        cursorX = CLI_LEFT + 1;
        cursorY = CLI_TOP + 1;
    } else if (string_compare(command, "showinfo") == 0) { // Show board revision (value and information) and board MAC address in correct format
        // Board revision
        unsigned int rev = get_board_revision();
        cli_put_string("Board Revision: ", WHITE, ZOOM);
        cli_put_hex(rev,  WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);

        // MAC Address
        unsigned char mac[6];
        if (get_mac_address(mac)) {
            // Print MAC address
            cli_put_string("MAC Address: ", WHITE, ZOOM);
            for (int i = 0; i < 6; i++) { 
                if (i > 0) cli_put_char(':', WHITE, ZOOM);
                cli_put_hex8(mac[i], WHITE, ZOOM);
            }
        } else {
            cli_put_string("Failed to retrieve MAC address\n", WHITE, ZOOM);
        }
        cli_put_char('\n', WHITE, ZOOM);
    } else if (string_compare(command, "baudrate") == 0) { // Allow user to change baudrate of current UART
        cli_put_string("Supported rates: 9600, 19200, 38400, 57600, 115200.\n", WHITE, ZOOM);
        cli_put_string("Input baudrate: ", WHITE, ZOOM);
        char baud_input[MAX_BUFFER]; // store baudrate string
        int index = 0;
        while (index < MAX_BUFFER - 1) {
            char c = uart_getc();
            if (c == '\r' || c == '\n') { // Wait until "Enter"
                baud_input[index] = '\0';
                break;
            } 
            baud_input[index++] = c;
            cli_put_char(c, WHITE, ZOOM);  // Echo input
        }
        cli_put_char('\n', WHITE, ZOOM);
        int baudrate = convert(baud_input); // Convert string args to integer
        if (baudrate == 9600 || baudrate == 19200 || baudrate == 38400 ||
            baudrate == 57600 || baudrate == 115200) {
            set_baudrate(baudrate);
            cli_put_string("Baudrate is changed to: ", WHITE, ZOOM);
            cli_put_string(baud_input, WHITE, ZOOM);
            cli_put_char('\n', WHITE, ZOOM);
        } else {
            cli_put_string("Invalid baud rate. Supported rates: 9600, 19200, 38400, 57600, 115200.\n", WHITE, ZOOM);
        }
    } else if (string_compare(command, "handshake") == 0) { // turn on/off CTS/RTS handshake
        cli_put_string("Input 0 to disable handshake, 1 to enable handshake.\n", WHITE, ZOOM);
        cli_put_string("Input: ", WHITE, ZOOM);
        char handshake_input[MAX_BUFFER]; // store baudrate string
        int index = 0;
        while (index < MAX_BUFFER - 1) {
            char c = uart_getc();
            if (c == '\r' || c == '\n') { // Wait until "Enter"
                handshake_input[index] = '\0';
                break;
            } 
            handshake_input[index++] = c;
            cli_put_char(c, WHITE, ZOOM);  // Echo input
        }
        cli_put_char('\n', WHITE, ZOOM);
        int status = convert(handshake_input); // Convert string args to integer
        if (status == 1 || status == 0) {
            uart_set_flow_control(status);  // Correct function name
        
            cli_put_string("Handshake ", WHITE, ZOOM);
            cli_put_string(status ? "enabled" : "disabled", WHITE, ZOOM);
            cli_put_string("\n", WHITE, ZOOM);
        } else {
            cli_put_string("Invalid input. Input 0 to disable handshake, 1 to enable handshake.\n", WHITE, ZOOM);
        }
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command, "video") == 0) {
        draw_background(); 
        video_playback(video_allArray, video_allArray_LEN, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, DESTINATION_WIDTH, DESTINATION_HEIGHT);
    } else if(string_compare(command, "game") == 0) {
        game_loop();
    } else {
        // Default: command not recognized
        const char *msg = "Unknown command\n";
        for (int i = 0; msg[i] != '\0'; i++) {
            cli_put_char(msg[i], WHITE, ZOOM);
        }
    }
}

void show_all_help(){
    cli_put_string("help - Show brief information of all command", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("clear - Clear screen", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("showinfo - Show board revision and MAC address", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("baudrate - change the baudrate of UART", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("handshake - turn on/off CTS/RTS handsharking on current UART", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("video - play video in a loop", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
    cli_put_string("game - turn on the video game", WHITE, ZOOM);
    cli_put_char('\n', WHITE, ZOOM);
}

void show_command_help(char* command_name){
    if(string_compare(command_name, "help") == 0){
        cli_put_string("help - Show brief information of all command", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "clear") == 0){
        cli_put_string("clear - clear the cli screen", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "showinfo") == 0){
        cli_put_string("showinfo - Show board revision and MAC address", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "baudrate") == 0){
        cli_put_string("baudrate - change the baudrate of UART", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "handshake") == 0){
        cli_put_string("handshake - turn on/off CTS/RTS handsharking on current UART ", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "video") == 0){
        cli_put_string("video - play video in a loop", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    } else if(string_compare(command_name, "game") == 0){
        cli_put_string("game - turn on the video game", WHITE, ZOOM);
        cli_put_char('\n', WHITE, ZOOM);
    }
}

void video_playback(const unsigned long** frames, uint32_t frame_count, int x, int y, int src_width, int src_height, int max_width, int max_height) {
    uint32_t current_frame = 0;
    
    // Initialize timer for first frame
    set_wait_timer(1, FRAME_US);

    while (current_frame < frame_count) {
        
        // detect non-blocking input 
        if(getUart() == ESC){ 
            draw_background();
            draw_cli_window();
            // Reset cursor position
            cursorX = CLI_LEFT + 1;
            cursorY = CLI_TOP + 1;
            break;
        }
        // 1. Display current frame (implement your display function)
        drawImageScaledAspect(x, y, frames[current_frame], src_width, src_height, max_width, max_height);
        // 2. Wait for next frame time
        set_wait_timer(0, 0); // Uses previously set expiration time
        
        // 3. Prepare timer for next frame
        set_wait_timer(1, FRAME_US);
        
        // 4. Advance to next frame (with optional loop handling)
        current_frame++;
        if (current_frame >= frame_count) {
            current_frame = 0; // Loop video if desired
        }
    }
}

