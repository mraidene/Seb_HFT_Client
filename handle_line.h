#include <cstring>
#include <iostream>
#include "compare_char_bid_a_geq_b.h"
#include "compare_char_ask_a_leq_b.h"
#include "bid_and_ask.h"
#include "char_id_to_int.h"

inline bool handle_line(char* &line_start, char* end, BidAndAsk* best_quotes){
    if (!line_start || line_start == end) return false;
    char* ptr = line_start;
    if (*ptr != 'S') return false;
    char* bid = nullptr;
    char* ask = nullptr;

    // MAKE SURE THAT THE LINE IS COMPLETE AND LOCATE BID AND ASK WHILE DOING SO
    while (ptr < end && *ptr != '\n') {
        if (*ptr == 'D') bid = ptr;
        else if (*ptr == 'K') ask = ptr;
        ptr++;
    }

    if (ptr >= end) {
        return false;
    }

    char* id_num = line_start + 7 * sizeof(char);
    bid += 2;
    ask += 2;
    int ID = char_id_to_int(id_num);
    BidAndAsk* best_quote = &best_quotes[ID];
    // If the bid in this line is greater than the best bid, save this bid as the best bid
    if (compare_char_bid_a_geq_b(bid, best_quote -> bid)) best_quote -> bid = bid;

    // If the ask in this line is less than or equal to the best ask, save this bid as the best bid
    if (compare_char_ask_a_leq_b(ask, best_quote -> ask)) best_quote -> ask = ask;

    // IF WE HAVE COMPLETED THE LINE, WE MOVE THE POINTER TO THE NEXT LINE
    line_start = ptr + 1; 
    return true;
}