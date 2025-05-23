// main_engine.cpp
#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include <chrono>
#include <unordered_map>
#include <thread>                     // for std::this_thread::yield()
#include <boost/system/error_code.hpp>
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
        boost::asio::io_context io_context;

        // UDP setup
        const std::string udp_ip   = "239.255.0.1";
        const int         udp_port = 3001;
        udp::socket udp_socket = initialize_udp_connection(io_context, udp_ip, udp_port);

        // TCP setup
        const std::string tcp_ip   = "127.0.0.1";
        const int         tcp_port = 4000;
        auto tcp_socket = initialize_tcp_connection(io_context, tcp_ip, tcp_port);

        // Trader name
        char trader_name[name.length() + 1];
        const size_t trader_name_length = name.length();
        strcpy(trader_name, name.c_str());

        // Buffers and state
        int curr_slot = 0;
        char* buffer = (char*)calloc(slots * char_per_slot, sizeof(char));
        if (!buffer) {
            cerr << "Failed to allocate buffer memory!" << endl;
            return 1;
        }

        udp::endpoint sender_endpoint;
        const char*    end_flag = "GET:";
        const size_t   target_length = 7 * sizeof(char);

        char* output = (char*)malloc(80);
        strcpy(output, "CHALLENGE_RESPONSE ");

        while (true) {
            auto recycle_start = high_resolution_clock::now();
            // ... (you can keep your recycle timing code here) ...
            auto recycle_end = high_resolution_clock::now();
            auto dur = duration_cast<microseconds>(recycle_end - recycle_start);
            cout << "Total time to recycle the system: " << dur.count() << " us\n";

            // Prepare rotating slot
            char* buff_slot = buffer + curr_slot * char_per_slot;
            char* write     = buff_slot;
            char* processing_ptr = buff_slot;
            size_t buffer_length = 0;
            size_t remaining_space = char_per_slot;
            auto first_fragment_time = high_resolution_clock::now();
            auto last_fragment_time  = first_fragment_time;

            BidAndAsk best_quotes[1001];

            // Non-blocking receive until we see the "GET:" end_flag
            while (buffer_length == 0 ||
                   strncmp(buff_slot + buffer_length - 12, end_flag, 4) != 0)
            {
                boost::system::error_code ec;
                size_t added = udp_socket.receive_from(
                    boost::asio::buffer(write, remaining_space),
                    sender_endpoint, 0, ec);

                if (ec == boost::asio::error::would_block) {
                    this_thread::yield();
                    continue;
                }
                else if (ec) {
                    cerr << "Receive error: " << ec.message() << "\n";
                    break;
                }

                if (write == buff_slot) {
                    first_fragment_time = high_resolution_clock::now();
                }
                last_fragment_time = high_resolution_clock::now();

                buffer_length     += added;
                write             += added;
                remaining_space   -= added;

                // process any complete lines in this fragment
                while (handle_line(processing_ptr, write, best_quotes)) {}
            }

            // Extract target ID, quotes, build response
            char* target = buff_slot + buffer_length - target_length - 1;
            BidAndAsk q = best_quotes[char_id_to_int(target + 3)];
            char* output_curr = output + 19;

            add_challenge_id       (output_curr, buff_slot, buffer_length);
            add_ticker             (output_curr, target, target_length);
            add_bid                (output_curr, q.bid);
            add_ask                (output_curr, q.ask);
            add_trader_name_and_terminate(output_curr, trader_name, trader_name_length);

            // Send via TCP
            boost::asio::write(tcp_socket,
                               boost::asio::buffer(output,
                                                   output_curr - output));

            auto end_time = high_resolution_clock::now();
            auto d1 = duration_cast<microseconds>(end_time - first_fragment_time);
            auto d2 = duration_cast<microseconds>(end_time - last_fragment_time);

            cout << "\nSENT: "
                 << string(output, output_curr - output) << "\n"
                 << "Time from first fragment to response: "
                 << d1.count() << " us\n"
                 << "Time from last fragment to response:  "
                 << d2.count() << " us\n";

            curr_slot = (curr_slot + 1) % slots;
        }
    }
    catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}

int main() {
    trade(2, 50000, "SebsBoys");
    return 0;
}
