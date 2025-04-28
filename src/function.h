#ifndef FUNCTION_H
#define FUNCTION_H

// Function to reset array
void reset_arr(char* arr);

// Function to compare strings
int comp_str(const char* s1, const char* s2);

// Function to return absolute int value
int abs(int x);

// Function to convert integer to string
void integer_to_string(int n, char *str);

// Function to find the length of a string
int string_length(char *str);

// Function to copy memory
void *memcpy(void *dest, const void *src, unsigned long n);

#endif // FUNCTION_H