#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include <chrono>
#include <unordered_map>
#include "add_challenge_id.h"
#include "add_ticker.h"
#include "add_bid.h"
#include "add_ask.h"
#include "add_trader_name_and_terminate.h"
#include "initialize_udp_connection.h"
#include "initialize_tcp_connection.h"
#include "handle_line.h"
using boost::asio::ip::udp;
using namespace std;
using namespace chrono;

int trade(int slots, int char_per_slot, std::string name) {
    try {
        // Initializing io_context
        boost::asio::io_context io_context;

        // Initializing UDP Connection
        const std::string udp_ip = "239.255.0.1";
        const int udp_port = 3001;
        udp::socket udp_socket = initialize_udp_connection(io_context, udp_ip, udp_port);

        // Initializing TCP Connection ("127.0.0.1" for local testing)
        const std::string tcp_ip = "127.0.0.1";
        const int tcp_port = 4000;
        tcp::socket tcp_socket = initialize_tcp_connection(io_context, tcp_ip, tcp_port);

        // Converting Trader Name to Char*
        char trader_name[name.length() + 1];
        const size_t trader_name_length = sizeof(trader_name);
        strcpy(trader_name, name.c_str());

        // Initialize Buffer and Related Constants;
        int curr_slot = 0;
        char* buffer = (char*)malloc(slots * char_per_slot * sizeof(char));
        if (buffer == nullptr) {
            std::cerr << "Failed to allocate buffer memory!" << std::endl;
            return 1;
        }
        udp::endpoint sender_endpoint;
        // This is the string that we use to flag when we have the complete input from the server
        const char* end_flag = "GET:";

        // Initialize Output Char* and related constants
        const size_t target_length = 7 * sizeof(char); // This is the size of the targetID that we are looking for (Ex: SEC0001)
        char* output = (char*)malloc(40);
        strcpy(output, "CHALLENGE_RESPONSE "); // Putting the first bit that never changes into the output buffer

        // Handling Each Challenge
        while (true) {
            auto recycle_start = high_resolution_clock::now();
            // Moving output_curr Pointer to the challenge ID location
            char* output_curr = output + 19 * sizeof(char);

            // Copy Null Map for Storing Bids and Asks
            BidAndAsk best_quotes[1001];

            // Creating a pointer to the current slot in the rotating buffer
            char* buff_slot = buffer + curr_slot * char_per_slot * sizeof(char);
            char* write = buff_slot;
            char* processing_ptr = buff_slot;

            // This array of char pointers first contains a pointer to the target, a pointer to the best bid, and a pointer to the best ask
            size_t buffer_length = 0;
            size_t added_length = 0;
            size_t remaining_space_in_slot = char_per_slot * sizeof(char);
            auto recycle_end = high_resolution_clock::now();
            auto duration3 = duration_cast<microseconds>(recycle_end - recycle_start);
            std::cout << "Total time to recycle the system: " << duration3.count() << " us" <<std::endl;

            auto last_fragment_time = high_resolution_clock::now();
            auto first_fragment_time = high_resolution_clock::now();


            // Receiving Message from the Server as Fragments
            while (buffer_length == 0 || strncmp(buff_slot + buffer_length - 12 * sizeof(char) , end_flag, 4*sizeof(char)) != 0) {
                added_length = udp_socket.receive_from(boost::asio::buffer(write, remaining_space_in_slot), sender_endpoint);
                if (write == buff_slot){
                    first_fragment_time = high_resolution_clock::now();
                }
                last_fragment_time = high_resolution_clock::now();
                buffer_length += added_length;
                write += added_length;
                remaining_space_in_slot -= added_length;
                // Processes each incoming line and returns true if a complete line was processed
                // If a complete line was processed, processing_ptr is advanced to the next line
                // If a complete line is not processed (meaning an incomplete input / waiting for next fragment)
                // returns false and doese not advance processing_ptr, thus waiting for the next fragment 
                while (handle_line(processing_ptr, write, best_quotes)){}
            }

            // Finding components and saving their locations as ptrs - waiting for complete message until processing
            char* target = buff_slot + buffer_length - target_length - sizeof(char);

            BidAndAsk best_quote = best_quotes[char_id_to_int(target+3*sizeof(char))];
            // Constructing Output and Moving output_curr Pointer to Next Component With Each Line
            add_challenge_id(output_curr, buff_slot, buffer_length);
            add_ticker(output_curr, target, target_length);
            add_bid(output_curr, best_quote.bid);
            add_ask(output_curr, best_quote.ask);
            add_trader_name_and_terminate(output_curr, trader_name, trader_name_length);
            // Sending Messages to the Server
            boost::asio::write(tcp_socket, boost::asio::buffer(output, output_curr - output));
            auto end = high_resolution_clock::now();
            auto duration1 = duration_cast<microseconds>(end - first_fragment_time);
            auto duration2 = duration_cast<microseconds>(end - last_fragment_time);

            std::cout << '\n' << "SENT: " << std::string(output, output_curr - output);
            std::cout << "Time from first fragment to response: " << duration1.count() << " us" <<std::endl;
            std::cout << "Time from last fragment to response: " << duration2.count() << " us" <<std::endl;

            // Resetting everything after completing response
            curr_slot = (curr_slot + 1) % slots;
        }
    } catch (std::exception& e) {
        // If Anything Goes Wrong!
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main() { 
    trade(1, 40000, "SebsBoys");
    return 0;
}