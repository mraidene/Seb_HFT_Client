#include <cstring>
#include <iostream>

inline char* find_target_line(char* buffer, char* target, size_t buffer_length, size_t target_length){
    char* end = buffer + buffer_length;
    char* ptr = buffer;
    while (ptr < end){
        if (*ptr == 'S'){
            ptr += 4 * sizeof(char);
            if (strncmp(target, ptr, 7) == 0){
                return ptr;
            }
        } else {
            return nullptr;
        }
        while (ptr < end && *ptr != '\n') ptr++;
        ptr ++;
    }
    return nullptr;
}