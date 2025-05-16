#include <iostream>
#include <cstring>

inline char* add_ticker(char* output, char* ticker, size_t ticker_length){
    std::memcpy(output, ticker, ticker_length);
    output += ticker_length;
    *output++ = ' ';
    return output;
}