#include <boost/asio.hpp>

using boost::asio::ip::udp;

inline udp::socket initialize_udp_connection(boost::asio::io_context &io_context, const std::string& server_ip, int server_port){
    udp::endpoint listen_endpoint(udp::v4(), server_port);
    udp::socket socket(io_context);
    socket.open(listen_endpoint.protocol());
    socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket.bind(listen_endpoint);

    boost::asio::ip::address multicast_address = boost::asio::ip::make_address(server_ip);
    socket.set_option(boost::asio::ip::multicast::join_group(multicast_address));
    return socket;
}