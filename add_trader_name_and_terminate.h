#include <cstring>
#include <iostream>

inline char* add_trader_name_and_terminate(char* output, const char* trader_name, size_t trader_name_length) {
    std::memcpy(output, trader_name, trader_name_length - sizeof(char));
    output += trader_name_length - sizeof(char);
    *output++ = '\n';
    *output++ = '\0';
    return output;
}