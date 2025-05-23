#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::udp;

inline udp::socket initialize_udp_connection(
    boost::asio::io_context& io_context,
    const std::string& server_ip,
    int server_port)
{
    udp::endpoint listen_endpoint(udp::v4(), server_port);
    udp::socket socket(io_context);

    socket.open(listen_endpoint.protocol());

    // allow reuse on this port
    socket.set_option(boost::asio::socket_base::reuse_address(true));

    socket.bind(listen_endpoint);

    socket.set_option(boost::asio::socket_base::receive_buffer_size(4 * 1024 * 1024));
    socket.set_option(boost::asio::socket_base::send_buffer_size   (4 * 1024 * 1024));
    socket.non_blocking(true);

    // join the multicast group
    boost::asio::ip::address multicast_address = boost::asio::ip::make_address(server_ip);
    socket.set_option(boost::asio::ip::multicast::join_group(multicast_address));

    return socket;
}
