#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include <chrono>
#include "find_best_bid_and_ask.h"
#include "add_challenge_id.h"
#include "add_ticker.h"
#include "add_bid.h"
#include "add_ask.h"
#include "add_trader_name_and_terminate.h"
#include "initialize_udp_connection.h"
#include "initialize_tcp_connection.h"

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
        udp::endpoint sender_endpoint;
        const char* end_flag = "GET:";

        // Initialize Output Char* and related constants
        const size_t target_length = 7 * sizeof(char);
        char* output = (char*)malloc(40);
        strcpy(output, "CHALLENGE_RESPONSE ");

        // Handling Each Challenge
        while (true) {
            // Moving output_curr Pointer to the challenge ID location
            char* output_curr = output + 19 * sizeof(char);

            // Creating a pointer to the current slot in the rotating buffer
            char* buff_slot = buffer + curr_slot * char_per_slot * sizeof(char);
            char* write = buff_slot;

            // This array of char pointers first contains a pointer to the target, a pointer to the best bid, and a pointer to the best ask
            char* ptrs[3] = {nullptr};
            size_t buffer_length = 0;
            size_t added_length = 0;
            auto start = high_resolution_clock::now();
            // Receiving Message from the Server as Fragments
            while (buffer_length == 0 || strncmp(buff_slot + buffer_length - 12 * sizeof(char) , end_flag, 4*sizeof(char)) != 0) {
                added_length = udp_socket.receive_from(boost::asio::buffer(write, char_per_slot), sender_endpoint);
                if (write == buff_slot) {
                    start = high_resolution_clock::now();
                }
                buffer_length += added_length;
                write += added_length;
            }
            auto last_frag = high_resolution_clock::now();

            // Finding components and saving their locations as ptrs - waiting for complete message until processing
            ptrs[0] = buff_slot + buffer_length - target_length - sizeof(char);
            find_best_bid_and_ask(buff_slot, ptrs[0], buffer_length, target_length ,ptrs[1], ptrs[2]);

            // Constructing Output and Moving output_curr Pointer to Next Component With Each Line
            add_challenge_id(output_curr, buff_slot, buffer_length);
            add_ticker(output_curr, ptrs[0], target_length);
            add_bid(output_curr, ptrs[1]);
            add_ask(output_curr, ptrs[2]);
            add_trader_name_and_terminate(output_curr, trader_name, trader_name_length);

            // Sending Messages to the Server
            boost::asio::write(tcp_socket, boost::asio::buffer(output, output_curr - output));
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            auto wait_duration = duration_cast<microseconds>(last_frag - start);
            std::cout << std::string(buff_slot, buffer_length) << std::endl;
            std::cout << "SENT: " << std::string(output, output_curr - output) << std::endl;
            std::cout << "Time from first fragment to response: " << duration.count() << " us" <<std::endl;
            std::cout << "Time spent waiting for fragments: " << wait_duration.count() << " us" <<std::endl;

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
    trade(2, 40000, "SebsBoys");
    return 0;
}