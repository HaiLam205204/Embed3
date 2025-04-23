#include "../uart/uart0.h"
#include <string.h>

void cmd_help(const char *input) { // type 'help' shows menu of commands
    // Split the command to check for "help <command>"
    const char *arg = strchr(input, ' ');
    if (!arg) {
        uart_puts("Available commands:\n");
        uart_puts("  help [cmd]   - Show help info\n");
        uart_puts("  clear        - Clear the screen\n");
        uart_puts("  showinfo     - Show board info\n");
        uart_puts("  baudrate     - Change UART baudrate\n");
        uart_puts("  handshake    - Toggle UART CTS/RTS\n");
    } else {
        arg++;  // Skip space
        if (strcmp(arg, "clear") == 0)
            uart_puts("clear - Clear the screen\n");
        else if (strcmp(arg, "showinfo") == 0)
            uart_puts("showinfo - Display board revision and MAC address\n");
        else if (strcmp(arg, "baudrate") == 0)
            uart_puts("baudrate <rate> - Set UART baudrate (9600, 19200, ...)\n");
        else if (strcmp(arg, "handshake") == 0)
            uart_puts("handshake <on/off> - Enable/disable CTS/RTS handshaking\n");
        else
            uart_puts("Unknown command in help\n");
    }
}

// You don’t have screen memory, so simulate it by printing many newlines.
void cmd_clear() {
    for (int i = 0; i < 50; i++) {
        uart_puts("\n");
    }
}

//Assumes functions like get_board_revision() and get_mac_address() exist 
//(you may need to implement these via mailbox).
void cmd_showinfo() {
    unsigned int rev = get_board_revision();
    unsigned char mac[6];
    get_mac_address(mac);  // Must be implemented separately

    uart_puts("Board Revision: ");
    uart_hex(rev);
    uart_puts("\n");

    uart_puts("MAC Address: ");
    for (int i = 0; i < 6; i++) {
        if (i > 0) uart_sendc(':');
        uart_hex(mac[i]);
    }
    uart_puts("\n");
}

// Parses a number and updates UART settings 
// (you’ll need to expose a function like uart_set_baudrate() in your UART driver).
void cmd_baudrate(const char *input) {
    int rate = 0;
    const char *arg = strchr(input, ' ');
    if (arg) {
        rate = atoi(arg + 1);
    }

    if (rate == 9600 || rate == 19200 || rate == 38400 || rate == 57600 || rate == 115200) {
        uart_set_baudrate(rate);  // You need to implement this in your UART driver
        uart_puts("Baudrate changed\n");
    } else {
        uart_puts("Invalid baudrate. Supported: 9600, 19200, 38400, 57600, 115200\n");
    }
}

// Enable/disable hardware CTS/RTS if supported (basic stub here):
void cmd_handshake(const char *input) {
    const char *arg = strchr(input, ' ');
    if (!arg) {
        uart_puts("Usage: handshake on|off\n");
        return;
    }

    arg++;
    if (strcmp(arg, "on") == 0) {
        uart_enable_handshake();  // Stub function you implement
        uart_puts("CTS/RTS enabled\n");
    } else if (strcmp(arg, "off") == 0) {
        uart_disable_handshake();  // Stub function
        uart_puts("CTS/RTS disabled\n");
    } else {
        uart_puts("Invalid argument. Use 'on' or 'off'\n");
    }
}


void execute_command(const char *input) {
    if (strncmp(input, "help", 4) == 0) {
        cmd_help(input);  // you can pass whole input to check for `help <command>`
    } else if (strcmp(input, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(input, "showinfo") == 0) {
        cmd_showinfo();
    } else if (strncmp(input, "baudrate", 8) == 0) {
        cmd_baudrate(input);
    } else if (strncmp(input, "handshake", 9) == 0) {
        cmd_handshake(input);
    } else {
        uart_puts("Unknown command\n");
    }
}
