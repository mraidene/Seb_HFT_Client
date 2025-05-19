#include <iostream>
#include <cstring>

inline void add_ticker(char* &output, char* ticker, size_t ticker_length){
    std::memcpy(output, ticker, ticker_length);
    output += ticker_length;
    *output++ = ' ';
}