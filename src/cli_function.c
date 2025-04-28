#include "uart.h"
#include "cli_function.h"
#include "function.h"
#include "mbox.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#define BACKSPACE '\b'      // ASCII backspace character
#define EMPTY_CHAR ' '      // ASCII space character

static const char* commands[] = { "help", "clear", "showinfo", "baudrate", "stopbit", "game" };
#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

// Clear the line in the terminal and reset the cursor
void clear_line() {
    uart_puts("\r\033[2K");  // Clears the entire line and moves cursor to the beginning
    uart_puts("MyOS>: "); // Re-display the prompt
}

// Function for handling backspace
void backspace(char* cli_buffer, int* index) {
    (*index)--;                     // Roll back
    cli_buffer[*index] = '\0';       // Add null character to the previous index
    uart_sendc(EMPTY_CHAR);          // Send empty character (visual space)
    uart_sendc(BACKSPACE);           // Send backspace character
}

// Function to clear the CLI line
void clear_cli_line(char* cli_buffer, int* index) {
    while (*index > 0) {
        uart_sendc(BACKSPACE);
        backspace(cli_buffer, index);
    }
}

// Function to copy the CLI buffer
void copy_cli_buffer(char* cli_buffer, char* temp) {
    while (*cli_buffer != '\0') {
        *temp = *cli_buffer;
        cli_buffer++;
        temp++;
    }
    *temp = '\0';
}

// Function to record the command to a buffer
void record_buffer(char* cli_buffer, char* temp, int* index) {
    while (*cli_buffer != '\0') {
        *temp = *cli_buffer;
        cli_buffer++;
        temp++;
        (*index)++;
    }
    *temp = '\0';
}

// Function to remove leading and trailing whitespaces
void remove_whitespace(char* str) {
    int i, j;

    // Remove leading whitespaces
    for (i = 0; str[i] == ' ' || str[i] == '\t'; i++);

    for (j = 0; str[i]; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';
}

// Function for auto completion
void TAB(char* cli_buffer, int* index) {
    int last_space_index = -1;

    // Find the last space in the command
    for (int i = 0; i < *index; i++) {
        if (cli_buffer[i] == ' ') {
            last_space_index = i;
        }
    }

    char* partial_input = (last_space_index == -1) ? cli_buffer : (cli_buffer + last_space_index + 1);
    int input_len = *index - (last_space_index + 1);
    const char* matched_command = NULL;
    int matched_count = 0;

    // Search for matching commands
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (strncmp(commands[i], partial_input, input_len) == 0) {
            matched_count++;
            if (matched_count == 1) {
                matched_command = commands[i];
            } else {
                uart_puts("\nAmbiguous command, type more characters.\n");
                return;
            }
        }
    }

    // If exactly one match is found, autocomplete
    if (matched_count == 1) {
        clear_line();
        cli_buffer[last_space_index + 1] = '\0';           // Clear the part after the last space
        strcpy(cli_buffer + last_space_index + 1, matched_command); // Append matched command
        *index = last_space_index + 1 + strlen(matched_command);    // Update index
        uart_puts(cli_buffer);                             // Display the autocompleted command
    } else if (matched_count == 0) {
        uart_puts("\nNo matches found.\n");
    }
}

// Function to show board revision and MAC address
void showInfo(){
    
    mBuf[0] = 12*4; // Message Buffer Size in bytes (17 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    
    mBuf[2] = 0x00010002; // TAG Identifier: • Board revision
    mBuf[3] = 4; 
    mBuf[4] = 0; 
    mBuf[5] = 0; 

    mBuf[6] = 0x00010003; // TAG Identifier: • MAC address
    mBuf[7] = 8; 
    mBuf[8] = 0; 
    mBuf[9] = 0; 
    mBuf[10] = 0;

    mBuf[11] = MBOX_TAG_LAST;
    
    //Note: Board model and Board serial may give 0 values on QEMU. 
    //Board revision, board MAC address: try it yourself, since it is assignment tasks.

    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
        uart_puts("\n+ Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\nDATA: Board revision = ");
        uart_hex(mBuf[5]);

        uart_puts("\n+ Response Code in Message TAG: ");
        uart_hex(mBuf[8]);
        uart_puts("\nDATA: Board MAC Address = ");

        // Assume mBuf[9] contains the MAC address split across two 32-bit values
        uint32_t mac_hi = mBuf[9];    // High 32 bits
        uint32_t mac_lo = mBuf[10];   // Low 32 bits

        // Extract and format the MAC address as XX:XX:XX:XX:XX:XX
        uart_hex((mac_hi >> 24) & 0xFF); uart_puts(":");
        uart_hex((mac_hi >> 16) & 0xFF); uart_puts(":");
        uart_hex((mac_hi >> 8) & 0xFF); uart_puts(":");
        uart_hex(mac_hi & 0xFF); uart_puts(":");
        uart_hex((mac_lo >> 8) & 0xFF); uart_puts(":");
        uart_hex(mac_lo & 0xFF);

    } else {
        uart_puts("Unable to query!\n");
    }
}

// Function to handle the baudrate input by
void handle_baudrate_command(const char *args) {
    int baudrate = str_to_int(args); // Convert string args to integer
    if (baudrate == 9600 || baudrate == 19200 || baudrate == 38400 ||
        baudrate == 57600 || baudrate == 115200) {
        set_baudrate(baudrate);
    } else {
        uart_puts("Invalid baud rate. Supported rates: 9600, 19200, 38400, 57600, 115200.\n");
    }
}

// Function to execute commands
void execute_command(char* cli_buffer) {
    // Check for specific 'help' command
    if (strncmp(cli_buffer, "help ", 5) == 0) {
        const char* command_name = cli_buffer + 5; // Skip "help " part

        if (strncmp(command_name, "help", strlen("help")) == 0) {
            uart_puts("help - Show brief information of all commands\n");
            uart_puts("clear - Clear screen\n");
            uart_puts("showinfo - Show board revision and MAC address\n");
            uart_puts("baudrate <baudrate>- Change the baudrate\n");
            uart_puts("stopbit - Change the stopbit setting\n");
            uart_puts("game - Play game\n");
            uart_puts("picvid - display picture and video\n");
        } else if (strncmp(command_name, "clear", strlen("clear")) == 0) {
            uart_puts("clear - Clear the terminal screen.\n");
        } else if (strncmp(command_name, "showinfo", strlen("showinfo")) == 0) {
            uart_puts("showinfo - Show board revision and MAC address.\n");
        } else if (strncmp(command_name, "baudrate", strlen("baudrate")) == 0) {
            uart_puts("baudrate <baudrate>- Change the baud rate of UART.\n");
        } else if (strncmp(command_name, "stopbit", strlen("stopbit")) == 0) {
            uart_puts("stopbit <1|2> - Set UART stop bits to 1 or 2.\n");
        } else if (strncmp(command_name, "game", strlen("game")) == 0) {
            uart_puts("game - play game\n");
        }else if (strncmp(command_name, "picvid", strlen("picvid")) == 0) {
            uart_puts("picvid - display picture and video\n");
        }else {
            uart_puts("Unknown command for help. Type 'help' for a list of commands.\n");
        }
    } else if (strncmp(cli_buffer, "help", strlen("help")) == 0) {
        // Print general help information
        uart_puts("Available commands:\n");
        uart_puts("help - Show brief information of all commands\n");
        uart_puts("help <command_name> - Show full information of a specific command\n");
        uart_puts("clear - Clear screen\n");
        uart_puts("showinfo - Show board revision and MAC address\n");
        uart_puts("baudrate <baudrate>- Change the baudrate\n");
        uart_puts("stopbit <1|2> - Set UART stop bits to 1 or 2.\n");
        uart_puts("game - Play game\n");
        uart_puts("picvid - display picture and video\n");
    } else if (strncmp(cli_buffer, "clear", strlen("clear")) == 0) {
        uart_puts("\033[2J");  // Clear screen ANSI escape code
        uart_puts("\033[H");   // Move cursor to home position
    } else if (strncmp(cli_buffer, "showinfo", strlen("showinfo")) == 0) {
        showInfo();  // Call the actual showInfo function
    } else if (strncmp(cli_buffer, "baudrate", strlen("baudrate")) == 0) {
        handle_baudrate_command(cli_buffer + 9); // Handle baudrate command
    } else if (strncmp(cli_buffer, "stopbit", strlen("stopbit")) == 0) {
        uart_puts("Unable to change stopbit due to usage of Mini UART");
    } else if (strncmp(cli_buffer, "game", strlen("game")) == 0) {
        main_game();  
    }else if (strncmp(cli_buffer, "picvid", strlen("picvid")) == 0) {
        pic_display();
    } else {
        uart_puts("Unknown command. Type 'help' for a list of commands.\n");
    }
}

// Function to set baudrate
void set_baudrate(int baudrate) {
    unsigned int baud_reg_value = (250000000 / (baudrate * 8)) - 1;  // Assuming a clock of 250MHz
    AUX_MU_BAUD = baud_reg_value;
    uart_puts("Baud rate set to ");
    uart_dec(baudrate);
    uart_puts(" bps\n");
}

