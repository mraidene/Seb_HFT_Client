#include <cstring>
#include <iostream>
#include "compare_char_bid_a_geq_b.h"
#include "compare_char_ask_a_leq_b.h"
#include "bid_and_ask.h"
#include "char_id_to_int.h"

// template<typename C>
// void test_custom(char* const& s, char const* d, C& ret)
// {
//   std::bitset<255> delims;
//   while(*d)
//   {
//     unsigned char code = *d++;
//     delims[code] = true;
//   }

//   char* beg = s;
//   bool in_token = false;
//   for(char* it = s; *it != '\0';it++)
//   {
//     if(delims[*it])
//     {
//       if(in_token)
//       {
//         ret.push_back(typename C::value_type(beg, it));
//         in_token = false;
//       }
//     }
//     else if( !in_token )
//     {
//       beg = it;
//       in_token = true;
//     }
//   }
//   if( in_token )
//     ret.push_back(typename C::value_type(beg, s.end()));
// }

// inline bool handle_line_split(char* &line_start, char* end, BidAndAsk* best_quotes){
//     if (!line_start || line_start == end) return false;
//     char* ptr = line_start;
//     if (*ptr != 'S') return false;
//     char* bid = nullptr;
//     char* ask = nullptr;
//     std::cout << "HANDLE CALLED\n";

//     // MAKE SURE THAT THE LINE IS COMPLETE AND LOCATE BID AND ASK WHILE DOING SO
//     while (ptr < end) {
//         std::cout << *ptr;
//         ptr++;
//     }

//     std::cout << "\nHANDLE DONE\n";
//     line_start = ptr + 1;
//     return false;
// }

// inline bool handle_line_split(char* &line_start, char* end, BidAndAsk* best_quotes)
// {
//     std::cout << "NEW HANDLE LINE"<< '\n';
    
//     std::string string_line_start(line_start);
//     std::vector<std::string> lines;
//     test_custom<std::vector<std::string>>(string_line_start, "\n", lines);

//     BidAndAsk* best_quote;
//     std::vector<char*> temp;
//     bool prev_sec = false, prev_ask = false, prev_bid = false;
//     int ID;
//     std::string bid, ask;
//     int count = 0;

//     for(auto line: lines)
//     {
//         test_custom<std::vector<char*>>(line, "|", temp);
//         count = 0;
//         bid = -1.0;
//         ask = -1.0;
//         for(auto elem: temp)
//         {
//             count += 1;
//             if(prev_sec)
//             {
//                 ID = stoi(elem.substr(3));
//                 best_quote = &best_quotes[ID];
//             }
//             if(prev_ask)
//             {
//                 ask = best_quote->ask;
//                 if(compare_char_ask_a_leq_b(ask, elem))
//                     ask = stod(elem);
//             }
//             if(prev_bid)
//             {
//                 bid = best_quote->bid;
//                 if(bid < stod(elem))
//                     bid = stod(elem);
//             }
//             prev_sec = elem == "SEC";
//             prev_ask = elem == "ASK";
//             prev_bid = elem == "BID";
//             std::cout << elem << '\n';
//         }
//         if(count == 6)
//         {
//             best_quote->ask = ask;
//             best_quote->bid = bid;
//             line_start += line.length(); // Might cause errors if there is a mismatch
//         }
//         else
//         {
//             return false;
//         }
//     }
//     return true;
//     std::cout << "NEW HANDLE LINE PRINTED ALL"<< '\n';
// }

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
    }

    // Just in case although not needed
    return true;
}

// inline bool handle_line(char* &line_start, char* end, BidAndAsk* best_quotes){
//     if (!line_start || line_start == end) return false;
//     char* ptr = line_start;
//     if (*ptr != 'S') return false;
//     char* bid = nullptr;
//     char* ask = nullptr;

//     // MAKE SURE THAT THE LINE IS COMPLETE AND LOCATE BID AND ASK WHILE DOING SO
//     while (ptr < end && *ptr != '\n') {
//         if (*ptr == 'D') bid = ptr;
//         else if (*ptr == 'K') ask = ptr;
//         ptr++;
//     }

//     if (ptr >= end) {
//         return false;
//     }

//     char* id_num = line_start + 7 * sizeof(char);
//     bid += 2;
//     ask += 2;
//     int ID = char_id_to_int(id_num);
//     BidAndAsk* best_quote = &best_quotes[ID];
//     // If the bid in this line is greater than the best bid, save this bid as the best bid
//     if (compare_char_bid_a_geq_b(bid, best_quote -> bid)) best_quote -> bid = bid;

//     // If the ask in this line is less than or equal to the best ask, save this bid as the best bid
//     if (compare_char_ask_a_leq_b(ask, best_quote -> ask)) best_quote -> ask = ask;

//     // IF WE HAVE COMPLETED THE LINE, WE MOVE THE POINTER TO THE NEXT LINE
//     line_start = ptr + 1; 
//     return true;
// }