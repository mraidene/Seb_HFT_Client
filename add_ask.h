#include <iostream>
#include <cstring>

inline void add_ask(char* &output, char* ask){
    while (*ask != '\n'){
        *output = *ask;
        ask++;
        output++;
    }
    *output++ = ' ';
}