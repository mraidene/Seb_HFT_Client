#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include "find_target_line.h"
#include "add_challenge_id.h"
#include "add_ticker.h"
#include "find_and_add_bid_and_ask.h"
#include "add_trader_name_and_terminate.h"
#include "initialize_udp_connection.h"
#include "initialize_tcp_connection.h"

using boost::asio::ip::udp;

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

        // Initialize Buffer
        int curr_slot = 0;
        char* buffer = (char*)malloc(slots * char_per_slot * sizeof(char));
        udp::endpoint sender_endpoint;

        // Initialize Output Char* and related constants
        const size_t target_length = 7 * sizeof(char);
        char* output = (char*)malloc(40);
        strcpy(output, "CHALLENGE_RESPONSE ");

        // Handling Each Challenge
        while (true) {
            // Moving output_curr Pointer to the challenge ID location
            char* output_curr = output + 19 * sizeof(char);

            // Creating a pointer to the current slot in the rotating buffer
            char* buff_slot = &buffer[curr_slot * char_per_slot];

            // Receiving Message from the Server
            size_t buffer_length = udp_socket.receive_from(boost::asio::buffer(buff_slot, char_per_slot), sender_endpoint);

            // Finding components and saving their locations as ptrs
            char* target = buffer + buffer_length - target_length - sizeof(char);
            char* target_line = find_target_line(buffer, target, buffer_length, target_length);

            // Constructing Output and Moving output_curr Pointer to Next Component With Each Line
            output_curr = add_challenge_id(output_curr, buffer, buffer_length);
            output_curr = add_ticker(output_curr, target, target_length);
            // Since BID and ASK have dynamic lengths, they will be found and added at the same time
            output_curr = find_and_add_bid_and_ask(output_curr, target_line, target_length);
            output_curr = add_trader_name_and_terminate(output_curr, trader_name, trader_name_length);
            // Sending Messages to the Server
            boost::asio::write(tcp_socket, boost::asio::buffer(output, std::strlen(output)));
            std::cout << "SENT: " << std::string(output) << std::endl;

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
    trade(1, 400, "SebsBoys");
    return 0;
}