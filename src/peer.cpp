#include "peer.h"

namespace core {

Peer::Peer(boost::asio::strand<boost::asio::thread_pool::executor_type>& strand,
           const boost::asio::ip::address& chat_room,
           const std::string& nickname)
    : socket_(strand)
    , room_endpoint_(chat_room, chatty_port_)
#ifndef _WIN64
    , stdin_(strand, ::dup(STDIN_FILENO))
#endif
    , nickname_(nickname)
{
    socket_.open(room_endpoint_.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(room_endpoint_);

    socket_.set_option(boost::asio::ip::multicast::join_group(chat_room));    
}

boost::asio::awaitable<void> Peer::do_receive(){
    while (true) {
        const auto [ec, n] = co_await socket_.async_receive_from(boost::asio::buffer(receiving_buffer_), remote_endpoint_, boost::asio::as_tuple(boost::asio::use_awaitable));

        if (ec.failed()) {
            co_return;
        }

        const auto received_message = std::string{receiving_buffer_.begin(), receiving_buffer_.begin() + n};

        if (received_message.find(nickname_) != 0) {
            std::cout.write(receiving_buffer_.data(), n);
            std::cout << '\n';
        }
    }
}

boost::asio::awaitable<void> Peer::do_send() {
#if defined(_WIN64)
    
#else 
    std::string nickname = nickname_;
    std::string message;
    std::string buffer = nickname.append(": " + message);
    
    auto welcome_message = std::string(nickname_ + " connected to the chat");

    const auto [ec, n] = co_await socket_.async_send_to(boost::asio::buffer(welcome_message), room_endpoint_, boost::asio::as_tuple(boost::asio::use_awaitable));
    if (ec.failed()) {
        co_return;
    }

    if (n > 0U) {
        std::cout << "Entered chat room successfully" << '\n';
    }

    while (true) {
        const auto [ec, n] = co_await boost::asio::async_read_until(stdin_, boost::asio::dynamic_buffer(sending_buffer_), '\n', boost::asio::as_tuple(boost::asio::use_awaitable));
        if (ec.failed()) {
            co_return;
        }
        
        const auto [ec1, n1] = co_await socket_.async_send_to(boost::asio::buffer(nickname_ + ": " + sending_buffer_), room_endpoint_, boost::asio::as_tuple(boost::asio::use_awaitable));
        if (ec1.failed()) {
            co_return;
        }
        
        std::cout << "You: " << sending_buffer_ << '\n';

        sending_buffer_.clear();
    }
#endif
}

} // namespace core
