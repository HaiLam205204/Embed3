// Prompt: MyOS>
// Buffer user input.
// Recognize special keys like \n, _, +, \t, and backspace.
// Handle enter to parse and execute.

#include "../uart/uart0.h"
#include "../header/commands.h"
#include "../header/history.h"
#include "../header/autocomplete.h"
#include "string.h"

#define PROMPT "MyOS> "
#define MAX_BUFFER 128

char input_buffer[MAX_BUFFER];
int buffer_index = 0;

void cli_start() {
    uart_puts(PROMPT); // display this while waiting for commands

    while (1) {
        char c = uart_getc(); // Repeatedly reads from the buffer for input

        if (c == '\n') { // Enter the input
            input_buffer[buffer_index] = '\0'; // store input inside a buffer
            uart_puts("\n"); // display new line as input is entered
            execute_command(input_buffer); // run the command entered
            add_to_history(input_buffer); // add the command to history
            buffer_index = 0; // reset the input buffer to receive new input
            uart_puts(PROMPT); 
        } else if (c == '\b' || c == 127) { // delete a character
            if (buffer_index > 0) {
                buffer_index--;
                uart_puts("\b \b"); // put a space
            }
        } else if (c == '_') { // get previous command in the history
            const char *cmd = get_prev_command();
            if (cmd) {
                while (buffer_index > 0) {
                    uart_puts("\b \b");
                    buffer_index--;
                }
                strcpy(input_buffer, cmd);
                buffer_index = strlen(cmd);
                uart_puts(input_buffer);
            }
        } else if (c == '+') { // get next command in the history
            const char *cmd = get_next_command();
            if (cmd) {
                while (buffer_index > 0) {
                    uart_puts("\b \b");
                    buffer_index--;
                }
                strcpy(input_buffer, cmd);
                buffer_index = strlen(cmd);
                uart_puts(input_buffer);
            }
        } else if (c == '\t') { // tab to autocomplete
            autocomplete(input_buffer, &buffer_index);
        } else {
            if (buffer_index < MAX_BUFFER - 1) {
                input_buffer[buffer_index++] = c;
                uart_sendc(c);
            }
        }
    }
}
