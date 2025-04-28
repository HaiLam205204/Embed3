#ifndef UTIL_H
#define UTIL_H


// Function declarations for string operations

int strlen(const char *s);

char *strcpy(char *dest, const char *src);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, int n);

// Function declarations for utility functions
void wait_msec(unsigned int n);
void displayInstructionsinTerminal();
void processUserInputinTerminal(char c);

#endif  // UTIL_H
