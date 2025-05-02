#include "../header/command.h"

void execute_command(char *command)
{
    // Check if the command starts with "help"
    if (compare_string_start(command, "help") == 0)
    {
        // Skip "help" and any leading spaces
        char *command_name = command + 4; // "help" has 4 characters
        while (*command_name == ' ')
        {
            command_name++;
        }

        // Pass the command name to help_command
        help_command(command_name);
    }
    else if (compare_string(command, "clear") == 0)
    {
        clear_command();
    }
    else if (compare_string_start(command, "setcolor") == 0)
    {
        // Skip "help" and any leading spaces
        char *color_command = command + 8; // "help" has 8 characters
        while (*color_command == ' ')
        {
            color_command++;
        }

        setcolor_command(color_command);
    }
    else if (compare_string(command, "showinfo") == 0)
    {
        showinfo_command();
    }
    else if (compare_string_start(command, "setbaudrate") == 0)
    {
        set_baud_rate_command(command + strlen("setbaudrate"));
    }
    else if (compare_string_start(command, "setdatabits") == 0)
    {
        set_data_bits_command(command + strlen("setdatabits"));
    }
    else if (compare_string_start(command, "setstopbits") == 0)
    {
        set_stop_bits_command(command + strlen("setstopbits"));
    }
    else if (compare_string_start(command, "setparity") == 0)
    {
        // Skip "setparity" and any leading spaces
        char *command_name = command + 9; // "setparity" has 9 characters
        while (*command_name == ' ')
        {
            command_name++;
        }
        set_parity_command(command_name);
    }
    else if (compare_string_start(command, "sethandshaking") == 0)
    {
        // Skip "sethandshaking" and any leading spaces
        char *command_name = command + 14; // "sethandshaking" has 14 characters
        while (*command_name == ' ')
        {
            command_name++;
        }
        set_handshaking_command(command_name);
    }
    else
    {
        uart_puts("\n\nCommand not found. Type 'help' in the terminal to see a list of supported commands in KimOS.\n\n");
    }
}

void help_command(char *command)
{
    uart_puts("\n\n");

    // Check if the command is "help" without any argument
    if (compare_string(command, "") == 0)
    {
        uart_puts("Command                                                      Description\n");
        uart_puts("---------------------------------------------------------------------------------------------------------------------\n");
        uart_puts("help                                                         Show brief information of all commands.\n\n");
        uart_puts("help <command_name>                                          Show full information of the command.\n\n");
        uart_puts("clear                                                        Clear screen.\n\n");
        uart_puts("setcolor -t <text color> -b <background color>               Set text color and/or background color.\n");
        uart_puts("                                                             Colors: black, red, yellow, blue, purple, cyan, white\n\n");
        uart_puts("showinfo                                                     Show board revision and board MAC address.\n\n");
        uart_puts("setbaudrate <baud rates>                                     Set desired baud rates.\n");
        uart_puts("                                                             Baud rates (Recommendations): 9600, 19200, 38400, 57600, 115200\n\n");
        uart_puts("setdatabits <the number of data bits>                        Set the number of data bits.\n");
        uart_puts("                                                             Data bits: 5, 6, 7, or 8\n\n");
        uart_puts("setstopbits <1 or 2>                                         Select between one or two stop bits.\n");
        uart_puts("                                                             Options: 1 or 2\n\n");
        uart_puts("setparity <none, even or odd>                                Configure parity.\n");
        uart_puts("                                                             Options: none, even, or odd\n\n");
        uart_puts("sethandshaking <on or off>                                   Set handshaking between CTS and RTS.\n");
        uart_puts("                                                             Options: on, off");
    }
    else
    {
        uart_puts("Command                                                      Description\n");
        uart_puts("---------------------------------------------------------------------------------------------------------------------\n");
        // Check the command name and display full information accordingly
        if (compare_string(command, "help") == 0)
        {
            uart_puts("help                                                         Show brief information of all commands.\n\n");
            uart_puts("                                                             Example: help\n");
            uart_puts("                                                             => This command will display brief information of all commands.\n\n");

            uart_puts("help <command_name>                                          Show full information of the command.\n");
            uart_puts("                                                             Example: help setcolor\n");
            uart_puts("                                                             => This command will display detailed information of setcolor command.");
        }
        else if (compare_string(command, "clear") == 0)
        {
            uart_puts("clear                                                        Clear screen.\n\n");
            uart_puts("                                                             Example: clear\n");
            uart_puts("                                                             => This command will clear the screen of the terminal.");
        }
        else if (compare_string(command, "setcolor") == 0)
        {
            uart_puts("setcolor -t <text color> -b <background color>               Set text color and/or background color of the terminal.\n");
            uart_puts("                                                             Options: black, red, yellow, blue, purple, cyan, white\n\n");
            uart_puts("                                                             Example: setcolor -t blue\n");
            uart_puts("                                                             => This command will change the text color of the terminal to blue.\n\n");
            uart_puts("                                                             Example: setcolor -b red\n");
            uart_puts("                                                             => This command will change the background color of the terminal to red.\n\n");
            uart_puts("                                                             Example: setcolor -b white -t green\n");
            uart_puts("                                                             => This command will change the background color of the terminal to white and the text color of it to green.\n");
            uart_puts("                                                             => The order of text color and background doesn't matter.");
        }
        else if (compare_string(command, "showinfo") == 0)
        {
            uart_puts("showinfo                                                     Show board revision and board MAC address.\n\n");
            uart_puts("                                                             Example: showinfo\n");
            uart_puts("                                                             => This command will display the revision and MAC address of the board.");
        }
        else if (compare_string(command, "setbaudrate") == 0)
        {
            uart_puts("setbaudrate <baud rates>                                     Set desired baud rates per second.\n");
            uart_puts("                                                             Recommendations: 9600, 19200, 38400, 57600, 115200\n");
            uart_puts("                                                             => The baud rates are not limited to the recommendations, though it will rely on the baud rates options of the terminal the user uses.\n");
            uart_puts("                                                             => Default: 115200\n");
            uart_puts("                                                             => Further Action: The baud rates in the terminal should be changed manually after the system configuration is done.\n\n");

            uart_puts("                                                             Example: setbaudrate 9600\n");
            uart_puts("                                                             => This command will change the baud rates of the system to 9600.");
        }
        else if (compare_string(command, "setdatabits") == 0)
        {
            uart_puts("setdatabits <the number of data bits>                        Set the number of data bits.\n");
            uart_puts("                                                             Options: 5, 6, 7, 8\n");
            uart_puts("                                                             => Default: 8\n");
            uart_puts("                                                             => Further Action: The data bits in the terminal should be changed manually after the system configuration is done.\n\n");
            uart_puts("                                                             Example: setdatabits 7\n");
            uart_puts("                                                             => This command will change the number of data bits to 7.");
        }
        else if (compare_string(command, "setstopbits") == 0)
        {
            uart_puts("setstopbits <1 or 2>                                         Select the number of stop bits.\n");
            uart_puts("                                                             Options: 1, 2\n");
            uart_puts("                                                             => Default: 2\n");
            uart_puts("                                                             => Further Action: The stop bits in the terminal should be changed manually after the system configuration is done.\n\n");
            uart_puts("                                                             Example: setstopbits 1\n");
            uart_puts("                                                             => This command will change the number of stop bits to 1.");
        }
        else if (compare_string(command, "setparity") == 0)
        {
            uart_puts("setparity <none, even or odd>                                Configure parity.\n\n");
            uart_puts("                                                             Options: none, even, odd\n");
            uart_puts("                                                             => Default: none\n");
            uart_puts("                                                             => Further Action: The parity in the terminal should be changed manually after the system configuration is done.\n\n");
            uart_puts("                                                             Example: setparity odd\n");
            uart_puts("                                                             => This command will change parity to odd.");
        }
        else if (compare_string(command, "sethandshaking") == 0)
        {
            uart_puts("sethandshaking <on or off>                                   Set handshaking between CTS and RTS.\n\n");
            uart_puts("                                                             Options: on, off\n");
            uart_puts("                                                             => Default: off\n");
            uart_puts("                                                             => Further Action: The handshaking mode in the terminal should be changed manually after the system configuration is done.\n\n");
            uart_puts("                                                             Example: sethandshaking on\n");
            uart_puts("                                                             => This command will turn on the handshaking mode between CTS and RTS.");
        }
        else
        {
            uart_puts("Command not found. Type 'help' in the terminal to see a list of supported commands in KimOS.");
        }
    }

    display_end();
}

void clear_command()
{
    uart_puts("\033[2J\033[H"); // ANSI escape sequence to clear the screen
}

void setcolor_command(char *command)
{
    char *token = command;
    char *text_color = (void *)0;
    char *background_color = (void *)0;
    int color_found = 0;

    while (*token != '\0')
    {
        // Check if the token starts with "-t"
        if (token[0] == '-' && token[1] == 't')
        {

            token += 2;

            while (*token == ' ')
            {
                token++;
            }

            if (*token == '\0')
            {
                uart_puts("\nInvalid command! Please specify a color after -t.\n");
                uart_puts("Loop Done\n");
                return;
            }

            text_color = token;

            while (*token != ' ' && *token != '\0')
            {
                token++;
            }

            *token = '\0';
            color_found = 1;
        }
        // Check if the token starts with "-b"
        else if (token[0] == '-' && token[1] == 'b')
        {

            token += 2;

            while (*token == ' ')
            {
                token++;
            }

            if (*token == '\0')
            {
                uart_puts("\nInvalid command! Please specify a color after -b.\n");
                uart_puts("Loop Done\n");
                return;
            }

            background_color = token;

            while (*token != ' ' && *token != '\0')
            {
                token++;
            }

            *token = '\0';
            color_found = 1;
        }
        token++;
    }

    if (color_found)
    {
        toUppercase(text_color);
        toUppercase(background_color);
        change_color(text_color, background_color);
        uart_puts("\nColor has changed!!\n\n");

        return;
    }

    uart_puts("\nInvalid command! Please specify at least one color.\n\n");
}

void showinfo_command()
{
    display_start("BOARD INFORMATION");
    get_mac_address_info();
    uart_puts("\n\n");
    get_revision_info();
    display_end();
}