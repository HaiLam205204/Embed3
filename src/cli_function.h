#ifndef CLI_FUNCTION_H
#define CLI_FUNCTION_H

// Character defines
#define EMPTY_CHAR 32
#define BACKSPACE 8
 
// Function for backspace
void backspace(char* cli_buffer, int* index);

// Function to clear the CLI line
void clear_cli_line(char* cli_buffer, int* index);

// Function to remove leading and trailing whitespaces
void remove_whitespace(char *str);

// Function to execute commands
void execute_command(char* cli_buffer);

#endif // CLI_FUNCTION_H