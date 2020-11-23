#include "tcp_connection.hh"

#include "event_loop.hh"
#include "logger.hh"
#include <experimental/source_location>
#include <unistd.h>

namespace m
{

tcp_connection::tcp_connection(event_loop*         loop,
                               const std::string&  name,
                               socket&             socket,
                               const inet_address& host_addr,
                               const inet_address& peer_addr)
    : channelable(loop, socket.fd())
    , name_(name)
    , state_(state_e::k_connecting)
    , socket_(socket)
    , host_addr_(host_addr)
    , peer_addr_(peer_addr)
{
    TRACE << "tcp connection: " << name_
          << " at socketfd: " << socket_.fd()
          << " is established for connection"
          << " from " << host_addr_.adress()
          << " to " << peer_addr_.adress();
}

tcp_connection::~tcp_connection()
{

    TRACE << "tcp_connection::~tcp_connection(): deleing " << name_;
}

void tcp_connection::connection_estabalished()
{

    assert_in_loop_thread();

    assert(state_ == state_e::k_connecting);
    set_state(state_e::k_connected);

    channel_enable_reading();

    connection_cb_(shared_from_this());
}

void tcp_connection::connection_destroyed()
{
    assert_in_loop_thread();

    TRACE << "tcp_connection::connection_destroyed "
          << "[" << name_ << "] ";

    set_state(state_e::k_disconnected);

    // remove the channel in poller
    channel_disable_all();
    unwatch_this_channel();

    connection_cb_(shared_from_this());
}

void tcp_connection::handle_write(const time_point_t& when)
{
}

void tcp_connection::handle_read(const time_point_t& recive_time)
{
    size_t readed_size = buffer_.read_fd(socket_.fd());
    auto   now         = clock::now();
    TRACE << "[" << name_ << "] "
          << "reads " << readed_size << " bytes";

    if (readed_size > 0)
    {
        message_cb_(shared_from_this(), buffer_, now);
    }
    else if (readed_size == 0)
    {
        handle_close(recive_time);
    }
    else
    {
        handle_error(recive_time);
    }
}

void tcp_connection::handle_close(const time_point_t& when)
{
    assert_in_loop_thread();
    assert(state_ == state_e::k_connected);
    channel_disable_all();
    close_cb_(shared_from_this());
}

void tcp_connection::handle_error(const time_point_t& when)
{
    int err = socket_.get_error();
    ERR << "tcp_connection::handle_error [" << name_
        << "] - SO_ERROR = " << err << " " << strerror(err);
}

} // namespace m
