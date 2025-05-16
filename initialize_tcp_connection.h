#include <boost/asio.hpp>

using boost::asio::ip::tcp;

inline tcp::socket initialize_tcp_connection(boost::asio::io_context &io_context, const std::string& server_ip, int server_port){
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(server_ip, std::to_string(server_port));
    tcp::socket tcp_socket(io_context);
    boost::asio::connect(tcp_socket, endpoints);
    return tcp_socket;
}