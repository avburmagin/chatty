#include "peer.h"

#include <boost/asio/io_context.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cerr << "Usage: ./chatty <your_nickname> <chat_room>" << std::endl;
        std::exit(1);
    }

    boost::asio::io_context io_context;
    boost::asio::ip::address chat_room(boost::asio::ip::make_address(argv[2]));
    core::Peer peer(io_context, chat_room, argv[1]);

    boost::asio::co_spawn(io_context, peer.do_receive(), boost::asio::detached);
    boost::asio::co_spawn(io_context, peer.do_send(), boost::asio::detached);

    io_context.run();

    return 0;
}
