#include "tcp_connection.hh"

#include <unistd.h>

#include "event_loop.hh"
#include "logger.hh"

namespace m
{

tcp_connection::tcp_connection(event_loop*         loop,
                               const std::string&  name,
                               socket&             socket,
                               const inet_address& host_addr,
                               const inet_address& peer_addr)
    : loop_(loop)
    , name_(name)
    , state_(state_e::k_connecting)
    , socket_(socket)
    , channel_(loop_, socket_.fd())
    , host_addr_(host_addr)
    , peer_addr_(peer_addr)
{
    TRACE << "tcp connection: " << name_
          << " at socketfd: " << socket_.fd()
          << " is established for connection"
          << " from " << host_addr_.adress()
          << " to " << peer_addr_.adress();

    channel_.set_read_callback([&] { handle_read(); });
}

tcp_connection::~tcp_connection()
{
    TRACE << "tcp_connection::~tcp_connection(): deleing " << name_;
}

void tcp_connection::connection_estabalished()
{

    loop_->assert_in_loop_thread();

    assert(state_ == state_e::k_connecting);
    set_state(state_e::k_connected);

    channel_.enable_reading();

    connection_cb_(shared_from_this());
}

void tcp_connection::connection_destroyed()
{
    loop_->assert_in_loop_thread();
    TRACE << "tcp_connection::connection_destroyed "
          << "[" << name_ << "] ";

    set_state(state_e::k_disconnected);

    // remove the channel in poller
    channel_.disable_all();
    loop_->remove_channel(&channel_);

    connection_cb_(shared_from_this());
}

void tcp_connection::handle_read()
{
    char    buf[65536];
    ssize_t readed_size = ::read(socket_.fd(), buf, sizeof buf);

    TRACE << "cp_connection::handle_read"
          << "[" << name_ << "] "
          << "reads " << readed_size << " bytes";

    if (readed_size > 0)
    {
        message_cb_(shared_from_this(), buf, readed_size);
    }
    else if (readed_size == 0)
    {
        handle_close();
    }
    else
    {
        handle_error();
    }
}

void tcp_connection::handle_close()
{
    loop_->assert_in_loop_thread();
    assert(state_ == state_e::k_connected);
    channel_.disable_all();
    close_cb_(shared_from_this());
}

void tcp_connection::handle_error()
{
    int err = socket_.get_error();
    ERR << "tcp_connection::handle_error [" << name_
        << "] - SO_ERROR = " << err << " " << strerror(err);
}

} // namespace m
