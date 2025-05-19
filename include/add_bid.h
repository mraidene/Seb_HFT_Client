#include <iostream>
#include <cstring>

inline void add_bid(char* &output, char* bid){
    while (*bid != '|'){
        *output = *bid;
        bid++;
        output++;
    }
    *output++ = ' ';
}
