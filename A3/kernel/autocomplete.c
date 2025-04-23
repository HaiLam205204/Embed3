#include "../header/autocomplete.h"
#include "../uart/uart0.h"
#include <string.h>

const char *commands[] = {
    "help", "clear", "showinfo", "baudrate", "handshake"
};

void autocomplete(char *buffer, int *index) {
    for (int i = 0; i < sizeof(commands)/sizeof(char*); i++) {
        if (strncmp(buffer, commands[i], *index) == 0) {
            int len = strlen(commands[i]);
            if (len > *index) {
                for (int j = *index; j < len; j++) {
                    buffer[*index] = commands[i][j];
                    uart_sendc(commands[i][j]);
                    (*index)++;
                }
            }
            break;
        }
    }
}
