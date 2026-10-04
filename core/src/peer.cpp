#include "chatty/core/peer.h"

namespace core {

Peer::Peer(boost::asio::io_context& io_context,
           const boost::asio::ip::address& chat_room,
           const std::string& nickname)
    : socket_(io_context)
    , room_endpoint_(chat_room, chatty_port_)
    , stdin_(io_context, ::dup(STDIN_FILENO))
    , nickname_(nickname)
{
    socket_.open(room_endpoint_.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(room_endpoint_);

    socket_.set_option(boost::asio::ip::multicast::join_group(chat_room));

    auto welcome_message = std::string(nickname_ + " connected to the chat");

    socket_.async_send_to(boost::asio::buffer(welcome_message), room_endpoint_,
                          [this](const boost::system::error_code& error_code, std::size_t bytes_sent){
        if (!error_code.failed() && bytes_sent > 0U){
            std::cout << "Entered chat room successfully" << std::endl;
        }
    });
}

void Peer::do_receive(){
    socket_.async_receive_from(boost::asio::buffer(receiving_buffer_), remote_endpoint_,
                          [this](const boost::system::error_code& error_code, std::size_t bytes_received){
        if (!error_code.failed() && bytes_received > 0){
            auto received_message_string = std::string(receiving_buffer_.begin(), receiving_buffer_.begin() + bytes_received);
            if (received_message_string.find(nickname_) != 0){
                std::cout.write(receiving_buffer_.data(), bytes_received);
                std::cout << "" << std::endl;
            }
            do_receive();
        }
    });
}

void Peer::do_send(){
    std::string nickname = nickname_;
    std::string message;
    std::string buffer = nickname.append(": " + message);
    
    boost::asio::async_read_until(stdin_, boost::asio::dynamic_buffer(sending_buffer_), '\n', [this](boost::system::error_code error_code, std::size_t n){
        if (error_code.failed()) {
            return;
        }

        socket_.async_send_to(boost::asio::buffer(nickname_ + ": " + sending_buffer_), room_endpoint_, [this](const boost::system::error_code& error_code, std::size_t){
            if (error_code.failed()) {
                return;
            }

            std::cout << "You sent: " << sending_buffer_ << '\n';

            sending_buffer_.clear();

            do_send();
        });
    });
}

} // namespace core
