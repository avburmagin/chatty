#include "chatty/core/peer.h"

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

    peer.do_receive();
    peer.do_send();

    io_context.run();

    return 0;
}
