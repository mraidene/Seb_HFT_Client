#include <iostream>
#include <cstring>

inline void add_challenge_id(char* &output, char* buffer, size_t buffer_length){
    char* ptr = buffer + buffer_length - 17 * sizeof(char);
    size_t id_length = 0;
    while (*ptr != ':'){
        ptr--;
        id_length++;
    }
    ptr++;
    std::memcpy(output, ptr, id_length);
    output += id_length;
    *output++ = ' ';
}
