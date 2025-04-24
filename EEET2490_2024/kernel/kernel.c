#include "../header/uart0.h"
#include "../header/util.h"
#include "../header/command.h"
#include "../header/ui.h"

// Command buffer
char command_buffer[MAX_COMMAND_LENGTH];
int buffer_index = 0;

// Command history
char command_history[MAX_HISTORY_SIZE][MAX_COMMAND_LENGTH];
int history_count = 0;
int history_index = 0;

// Command auto completion
char cur_prefix[20];
char last_autocompletion[20];

const char *command_list[] = {
    "help",
    "clear",
    "setcolor",
    "showinfo",
    "setbaudrate",
    "setdatabits",
    "setstopbits",
    "setparity",
    "sethandshaking"};

const int num_commands = sizeof(command_list) / sizeof(command_list[0]);

void main()
{

    uart_init(26, 3);
    display_welcome_msg();
    display_prompt();

    while (1)
    {
        // Read each char
        char c = uart_getc();

        if (c == '\n') // Enter key
        {
            // Add the command to history
            add_to_history(command_buffer);

            // Null-terminate the command buffer
            command_buffer[buffer_index++] = '\0';

            // Execute the command
            execute_command(command_buffer);

            // Reset indexes for next command
            buffer_index = 0;
            history_index = history_count - 1;

            copy_string(cur_prefix, "");
            copy_string(command_buffer, "");
            copy_string(last_autocompletion, "");

            // Display prompt for next command
            display_prompt();
        }
        else if (c == 0x08 || c == 0x7F) // Delete or backspace key
        {
            // Check if buffer_index is greater than 0 to avoid underflow
            if (buffer_index > 0)
            {
                // Move the cursor back one position
                uart_sendc(c);

                // Remove the last character
                buffer_index = buffer_index - 1;
                command_buffer[buffer_index] = '\0';
                cur_prefix[buffer_index] = '\0';
                last_autocompletion[buffer_index] = '\0';
            }
        }
        else if (c == '\t') // TAB key for autocompletion
        {
            int next_index = find_next_auto_completion();

            if (next_index != -1)
            {
                reset_command_line();
                clear_buffer();

                copy_string(last_autocompletion, command_list[next_index]);
                copy_string(command_buffer, command_list[next_index]);

                int length = strlen(command_buffer);
                buffer_index = length;

                uart_puts(last_autocompletion);
            }
        }
        else if (c == '_') // UP arrow key
        {
            if (history_index >= 0)
            {
                display_history();
                if (history_index != 0)
                    history_index--;
            }
        }
        else if (c == '+') // DOWN arrow key
        {
            if (history_index <= history_count - 1)
            {
                display_history();
                if (history_index != history_count - 1)
                    history_index++;
            }
        }
        else // Regular characters
        {

            // Add the character to the command buffer
            command_buffer[buffer_index] = c;
            cur_prefix[buffer_index] = c;
            copy_string(last_autocompletion, cur_prefix);

            buffer_index++;

            uart_sendc(c); // Echo back to console
        }
    }
}

void clear_buffer()
{
    for (int i = 0; i < MAX_COMMAND_LENGTH; i++)
    {
        command_buffer[i] = '\0';
    }

    buffer_index = 0;
}

void add_to_history(const char *command)
{
    // Update history count
    if (history_count >= MAX_HISTORY_SIZE)
    {
        // Shift commands in history to make space for the new command
        for (int i = MAX_HISTORY_SIZE - 1; i > 0; i--)
        {
            copy_string(command_history[i], command_history[i - 1]);
        }
        copy_string(command_history[history_count], command);
    }
    else
    {
        copy_string(command_history[history_count], command);
        history_count++;
    }
}

void display_history()
{
    reset_command_line();

    clear_buffer();

    const char *history = command_history[history_index];

    for (int i = 0; history[i] != '\0'; i++)
    {
        command_buffer[buffer_index++] = history[i];
    }

    uart_send_string(history);
}

int find_next_auto_completion()
{
    int skipped_index = 0;

    if (strlen(last_autocompletion) > 0)
    {
        for (int i = 0; i < num_commands; i++)
        {
            // check index to skip
            if (compare_string(last_autocompletion, command_list[i]) == 0)
            {
                skipped_index = i + 1;
                break;
            }
        }

        // if skipped index is at the end of the command list
        if (skipped_index == num_commands)
            skipped_index = 0;
    }

    for (int i = skipped_index; i < skipped_index + num_commands; i++)
    {
        skipped_index = i % num_commands;
        if (is_prefix(cur_prefix, command_list[i]) == 0)
        {
            return skipped_index;
        }
    }

    return -1; // No auto-completion found
}