#include <cstring>
#include <iostream>

inline char* find_and_add_bid_and_ask(char* output, char* target_line, size_t target_length) {
    target_line += target_length + 5 * sizeof(char);
    while (*target_line != '|'){
        *output++ = *target_line++;
    }
    *output++ = ' ';
    target_line += 5 * sizeof(char);
    while (*target_line != '\n'){
        *output++ = *target_line++;
    }
    *output++ = ' ';
    return output;
}