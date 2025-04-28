#include "util.h"


int strlen(const char *s) {
    int length = 0;
    while (*s != '\0') {
        length++;
        s++;
    }
    return length;
}


int strncmp(const char *s1, const char *s2, int n) {
    while (n > 0 && *s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) {
        return 0;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}


char *strcpy(char *dest, const char *src) {
    char *original_dest = dest;
    while ((*dest++ = *src++)) {
        // Copy each character including the null terminator
    }
    return original_dest;
}




void wait_msec(unsigned int msVal) {
    register unsigned long f, t, r, expiredTime; // 64 bits

    // Get the current counter frequency (Hz), 1Hz = 1 pulse/second
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter value
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expire value for counter
    expiredTime = t + f * msVal / 1000;

    do {
        asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
}


void processUserInputinTerminal(char c) {
    if (c != '\r') { // '\r' is the carriage return character (Enter key)
        // Position cursor below instructions
        uart_puts("\033[12;1H"); // Assuming instructions take up 11 lines

        // Clear the current line in the terminal
        uart_puts("\033[2K\r"); 

        // Print a message indicating the key pressed
        uart_puts("Key pressed: ");
        uart_sendc(c);
        uart_puts("\n");

        // Announce to the user if they restart the current level
        if (c == 'r') {
            uart_puts("Successfully restart the current level from the beginning");
        }
    }
}

