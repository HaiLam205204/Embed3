#include "../header/history.h"
#include <string.h>

#define HISTORY_SIZE 10
#define MAX_BUFFER 128

static char history[HISTORY_SIZE][MAX_BUFFER];
static int history_count = 0;
static int current_index = -1;

void add_to_history(const char *command) {
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count++], command);
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], command);
    }
    current_index = history_count; // reset index after adding
}

const char* get_prev_command() {
    if (history_count == 0 || current_index <= 0)
        return NULL;
    return history[--current_index];
}

const char* get_next_command() {
    if (history_count == 0 || current_index >= history_count - 1)
        return NULL;
    return history[++current_index];
}
