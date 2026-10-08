#include "peer.h"

#include <boost/asio.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cerr << "Usage: ./chatty <your_nickname> <chat_room>" << std::endl;
        std::exit(1);
    }

    boost::asio::thread_pool thread_pool{2};

    boost::asio::ip::address chat_room(boost::asio::ip::make_address(argv[2]));
    
    auto strand = boost::asio::make_strand(thread_pool.get_executor());

    core::Peer peer(strand, chat_room, argv[1]);
    boost::asio::co_spawn(strand, peer.do_receive(), boost::asio::detached);
    boost::asio::co_spawn(strand, peer.do_send(), boost::asio::detached);

    thread_pool.join();

    return 0;
}
