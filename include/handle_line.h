#include <cstring>
#include <iostream>
#include <thread>
#include "bid_and_ask.h"
#include "char_id_to_int.h"

void add_to_quotes(char* line_start, char* bid, char* ask, BidAndAsk* best_quotes)
{
    char* id_num = line_start + 7 * sizeof(char);
    bid += 2;
    ask += 2;
    int ID = char_id_to_int(id_num);
    BidAndAsk* best_quote = &best_quotes[ID];
    // If the bid in this line is greater than the best bid, save this bid as the best bid
    best_quote -> bid = bid;

    // If the ask in this line is less than or equal to the best ask, save this bid as the best bid
    best_quote -> ask = ask;
}

inline bool handle_line(char* &line_start, char* end, BidAndAsk* best_quotes){

    if (!line_start || line_start == end) return false;
    char* ptr = line_start;
    if (*ptr != 'S') return false;
    char* bid = nullptr;
    char* ask = nullptr;

    while(1)
    {
        // We've reached the end of the line and the ptr check below hasn't
        // returned false so that means that there were complete lines in
        // line_start

        if (!line_start || line_start == end) return true;
        ptr = line_start;
        if (*ptr != 'S') return false;

        // MAKE SURE THAT THE LINE IS COMPLETE AND LOCATE BID AND ASK WHILE DOING SO
        while (ptr < end && *ptr != '\n') {
            if (*ptr == 'D') bid = ptr;
            else if (*ptr == 'K') ask = ptr;
            ptr++;
        }

        if (ptr >= end) {
            return false;
        }

        // This makes a thread and leaves it running
        std::thread{add_to_quotes, line_start, bid, ask, best_quotes}.detach();

        // IF WE HAVE COMPLETED THE LINE, WE MOVE THE POINTER TO THE NEXT LINE
        line_start = ptr + 1;
    }

    // Just in case although not needed
    return true;
}
