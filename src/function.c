#include "uart.h"
#include "function.h"

// Function to reset array
void reset_arr(char* arr) {
    int i = 0;
    while (arr[i] != '\0') {
        arr[i] = '\0';
        i++;
    }
}

// Function to compare strings
int comp_str(const char* s1, const char* s2) {
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 < *s2) {
            return -1;
        } else if (*s1 > *s2) {
            return 1;
        }
        s1++;
        s2++;
    }

    if (*s1 == '\0' && *s2 == '\0') {
        return 0;   // Both strings are equal
    } else if (*s1 == '\0') {
        return -1;  // s1 is shorter
    } else {
        return 1;   // s2 is shorter
    }
}

// Function to convert integer to string
void integer_to_string(int n, char *str) {
    int i = 0;
    while (n != 0) {
        int rem = n % 10;
        str[i] = rem + '0';
        n = n / 10;
        i++;
    }
    str[i] = '\0';

    // Reverse the string
    int j = 0;
    int k = string_length(str) - 1;
    while (j < k) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
        j++;
        k--;
    }
}

// Function to find the length of a string
int string_length(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

int str_to_int(const char *str) {
    int result = 0;
    int i = 0;

    // Check for leading spaces
    while (str[i] == ' ') {
        i++;
    }

    // Parse each digit
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0'); // Convert char to digit and add to result
        i++;
    }

    return result;
}

// Function to copy memory
void *memcpy(void *dest, const void *src, unsigned long n) {
    for (unsigned long i = 0; i < n; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
}