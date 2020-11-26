#include "tcp_connection.hh"

#include "event_loop.hh"
#include "logger.hh"
#include <experimental/source_location>
#include <unistd.h>

namespace m
{

tcp_connection::tcp_connection(event_loop&         loop,
                               const std::string&  name,
                               const int           socket_fd,
                               const inet_address& host_addr,
                               const inet_address& peer_addr)
    : pollable(loop, socket_fd)
    , name_(name)
    , state_(state_e::connecting)
    , socket_(socket(socket_fd))
    , host_addr_(host_addr)
    , peer_addr_(peer_addr)
    , high_watermark_(64 * 1024 * 1024)
{
}

tcp_connection::~tcp_connection()
{
     TRACE << "tcp_connection: " << to_string() << " destoried.";
}

void tcp_connection::connection_estabalished()
{
    assert_in_loop_thread();

    assert(state_ == state_e::connecting);
    set_state(state_e::connected);

    channel_enable_reading();

    connection_cb_(shared_from_this());
}

void tcp_connection::connection_destroyed()
{
    assert_in_loop_thread();

    TRACE << "tcp_connection: " << to_string()  << " destorying.";

    set_state(state_e::disconnecting);

    // remove the channel in poller
    channel_disable_all();
    unwatch_this_channel();

    connection_cb_(shared_from_this());
}

void tcp_connection::send(const std::string& msg, const size_t len)
{
    size_t write_len = len > 0 ? len : msg.size();
    if ( state_ == state_e::connected )
    {
        if ( is_in_loop_thread() )
        {
            send_in_loop(msg, write_len);
        }
        else
        {
            owner_loop().run_in_loop(
                [&] { send_in_loop(msg, write_len); });
        }
    }
}

void tcp_connection::shotdown()
{
    if ( state_ == state_e::connected )
    {
        set_state(state_e::disconnecting);
        owner_loop().run_in_loop(
            [&, lifetime = shared_from_this()] { shotdown_in_loop(); });
    }
}

void tcp_connection::handle_read(const time_point& recive_time)
{
    size_t readed_size = inp_buffer_.read_fd(socket_.fd());
    auto   now         = clock::now();
    TRACE << "tcp_connection: " << to_string()
          << " reads " << readed_size << " bytes";

    if ( readed_size > 0 )
    {
        message_cb_(shared_from_this(), inp_buffer_, now);
    }
    else if ( readed_size == 0 )
    {
        handle_close(recive_time);
    }
    else
    {
        handle_error(recive_time);
    }
}

void tcp_connection::handle_write(const time_point& when)
{
    assert_in_loop_thread();
    if ( channel_is_writing() )
    {
        ssize_t num_write = ::write(fd(),
                                    oup_buffer_.peek(),
                                    oup_buffer_.writeable_bytes());
        if ( num_write > 0 )
        {
            oup_buffer_.retrieve(num_write);
            if ( oup_buffer_.readable_bytes() == 0 )
            {
                channel_disable_writing();

                if ( write_complete_cb_ )
                {
                    write_complete_cb_(shared_from_this());
                }

                if ( state_ == state_e::disconnecting )
                {
                    shotdown_in_loop();
                }
            }
            else
            {
                TRACE << "more data to write to " << peer_addr_;
            }
        }
    }
    else
    {
        TRACE << "conection to " << peer_addr_ << "is down";
    }
}

void tcp_connection::handle_close(const time_point& when)
{
    assert_in_loop_thread();
    assert(state_ == state_e::disconnecting);
    channel_disable_all();
    close_cb_(shared_from_this());
}

void tcp_connection::handle_error(const time_point& when)
{
    int err = socket_.get_error();
    ERR << "tcp_connection::handle_error [" << name_
        << "] - SO_ERROR = " << err << " " << strerror(err);
}

void tcp_connection::send_in_loop(const std::string& msg, const size_t num_total)
{
    assert_in_loop_thread();

    ssize_t num_write  = 0;
    ssize_t num_remain = num_total;

    // if nothing in the output queue, try directly write
    if ( !channel_is_writing() && oup_buffer_.readable_bytes() == 0 )
    {
        num_write  = socket_.write(msg.data(), num_total);
        if ( num_write > 0 )
        {
            num_remain -= num_write;
            if ( num_remain > 0 )
            {
                TRACE << "need to write more for " << fd();
            }
        }
        else
        {
            // when directly write falied
            num_write = 0;
            num_remain = num_total;
            if ( errno != EWOULDBLOCK )
            {
                ERR << "write to " << fd();
            }
        }
    }

    assert(num_remain <= num_total);
    if ( num_remain > 0)
    {
        ssize_t num_current_buffer = oup_buffer_.readable_bytes();
        ssize_t num_after_append   = num_current_buffer + num_remain;
        if ( high_watermark_cb_
             && num_after_append > high_watermark_
             && num_current_buffer < high_watermark_ )
        {
            high_watermark_cb_(shared_from_this(), num_current_buffer);
        }

        oup_buffer_.append(msg.data() + num_write, num_remain);
        if ( !channel_is_writing() )
        {
            channel_enable_writing();
        }
    }
    else if ( write_complete_cb_ )
    {
        owner_loop().queue_in_loop(
            [&] { write_complete_cb_(shared_from_this()); });
    }
}

void tcp_connection::shotdown_in_loop()
{
    assert_in_loop_thread();

    if ( !channel_is_writing() )
    {
        socket_.shutdown_write();
    }
}

} // namespace m
