#include <cstring>
#include <iostream>
#include "compare_char_bid_a_geq_b.h"
#include "compare_char_ask_a_leq_b.h"

inline void find_best_bid_and_ask(char* buffer, char* target, size_t buffer_length, size_t target_length, char* &best_bid, char* &best_ask){
    // best_bid and best_ask must initiall be null pointers
    char* end = buffer + buffer_length;
    char* ptr = buffer;
    while (ptr < end){
        if (*ptr == 'S'){
            ptr += 4 * sizeof(char);
            if (strncmp(target, ptr, 7) == 0){
                // Adjust ptr to point at the Bid
                ptr += 12 * sizeof(char);
                // If the bid in this line is greater than the best bid, save this bid as the best bid
                if (compare_char_bid_a_geq_b(ptr, best_bid)) best_bid = ptr;

                // Adjust ptr to point at the Ask
                while (*ptr != 'K') ptr++;
                ptr += 2 * sizeof(char);
                // If the ask in this line is less than or equal to the best ask, save this bid as the best bid
                if (compare_char_ask_a_leq_b(ptr, best_ask)) best_ask = ptr;
            }
        } else {
            return;
        }
        while (ptr < end && *ptr != '\n') ptr++;
        ptr ++;
    }
    return;
}