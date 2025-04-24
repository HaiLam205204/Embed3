#include "../header/util.h"

void copy_string(char *dest, const char *src)
{
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0'; // Add null terminator to the end of the destination string
}

int compare_string(const char *str1, const char *str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0') ? 0 : (*str1 - *str2);
}

int compare_string_start(const char *str, const char *prefix)
{
    while (*prefix && *str)
    {
        if (*str != *prefix)
        {
            return -1;
        }
        str++;
        prefix++;
    }
    // If prefix is empty, return true
    if (*prefix == '\0')
        return 0;

    return -1;
}

void toUppercase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] -= 32;
        }
    }
}

unsigned int strlen(const char *str)
{
    unsigned int length = 0;
    while (*(str++))
    {
        length++;
    }
    return length;
}

char *strcat(char *dest, const char *src)
{
    char *original_dest = dest;

    while (*dest)
    {
        dest++;
    }

    while (*src)
    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';

    return original_dest;
}

int atoi(const char *str)
{
    int result = 0;
    int sign = 1; // Sign of the number, initialize to positive
    int i = 0;

    // Skip leading whitespaces
    while (str[i] == ' ')
    {
        i++;
    }

    // Check for sign
    if (str[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (str[i] == '+')
    {
        i++;
    }

    // Convert digits to integer
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    // Apply sign
    return sign * result;
}

char hex_to_ascii(uint8_t value)
{
    // Convert value to ASCII character
    return value < 10 ? '0' + value : 'A' + (value - 10);
}

// Function to check if a string `prefix` is a prefix of string `str`
int is_prefix(const char *prefix, const char *str) {
    // Iterate through each character of `prefix`
    while (*prefix != '\0') {
        // If characters don't match, return false
        if (*prefix != *str) {
            return -1;
        }
        // Move to the next character in both strings
        prefix++;
        str++;
    }
    // If all characters in `prefix` match, return true
    return 0;
}
