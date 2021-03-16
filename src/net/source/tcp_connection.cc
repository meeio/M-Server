#include "tcp_connection.hh"

#include "event_loop.hh"
#include "logger.hh"
#include "poll_handle.hh"
#include <experimental/source_location>
#include <unistd.h>

namespace m
{

tcp_connection::tcp_connection(event_loop&        loop,
                               const std::string& name,
                               socket&&           sock)
    : name_(name)
    , state_(state_e::connecting)
    , socket_(std::forward<socket>(sock))
    , high_watermark_(64 * 1024 * 1024)
    , host_addr_(socket_.host_addr())
    , peer_addr_(socket_.peer_addr())
    , loop_(loop)
    , poll_hd_(nullptr)
{
    loop_.queue_in_loop([&]() {
        poll_hd_ = &loop_.register_polling(socket_.fd());
        poll_hd_->bind_handler(*this);
        connection_estabalished();
    });
}

tcp_connection::~tcp_connection()
{
    TRACE << "tcp_connection: " << to_string() << " destoried.";
}

void tcp_connection::connection_estabalished()
{
    loop_.assert_in_loop_thread();

    TRACE << to_string() << " estabalished.";

    assert(state_ == state_e::connecting);
    set_state(state_e::connected);

    poll_hd_->enable_reading();

    if (connection_cb_)
    {
        connection_cb_(shared_from_this());
    }
}

void tcp_connection::connection_destroyed()
{
    loop_.assert_in_loop_thread();

    TRACE << to_string() << " destroyed.";

    set_state(state_e::disconnected);

    connection_cb_(shared_from_this());
}

void tcp_connection::send(const std::string& msg, const size_t len)
{
    size_t write_len = len > 0 ? len : msg.size();
    if ( state_ == state_e::connected )
    {
        if ( loop_.is_in_loop_thread() )
        {
            send_in_loop(msg, write_len);
        }
        else
        {
            loop_.run_in_loop(
                [&] { send_in_loop(msg, write_len); });
        }
    }
}

void tcp_connection::shotdown()
{
    if ( state_ == state_e::connected )
    {
        set_state(state_e::disconnecting);
        loop_.run_in_loop(
            [&, lifetime = shared_from_this()] { shotdown_in_loop(); });
    }
}

void tcp_connection::handle_read(const time_point& recive_time)
{
    size_t readed_size = inp_buffer_.read_socket(socket_);
    auto   now         = clock::now();
    TRACE << to_string() << " reads " << readed_size << " bytes";

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

void tcp_connection::handle_write(const time_point&)
{
    loop_.assert_in_loop_thread();
    if ( poll_hd_->is_writing() )
    {
        ssize_t num_write = socket_.write(oup_buffer_.peek(),
                                          oup_buffer_.writeable_bytes());
        if ( num_write > 0 )
        {
            oup_buffer_.retrieve(num_write);
            if ( oup_buffer_.readable_bytes() == 0 )
            {
                poll_hd_->disable_writing();

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

void tcp_connection::handle_close(const time_point&)
{
    loop_.assert_in_loop_thread();
    assert(state_ == state_e::disconnecting
           || state_ == state_e::connected);

    poll_hd_->disable_all();
    
    close_cb_(shared_from_this());
    
    loop_.queue_in_loop([&, lifetime = shared_from_this()]() {
        loop_.remove_poll_handle(*poll_hd_);
        connection_destroyed();
    });
}

void tcp_connection::handle_error(const time_point&)
{
    int err = socket_.get_error();
    ERR << "tcp_connection::handle_error [" << name_
        << "] - SO_ERROR = " << err << " " << strerror(err);
}

void tcp_connection::send_in_loop(const std::string& msg,
                                  const size_t       num_total)
{
    loop_.assert_in_loop_thread();

    size_t num_write  = 0;
    size_t num_remain = num_total;

    // if nothing in the output queue, try directly write
    if ( !poll_hd_->is_writing() && oup_buffer_.readable_bytes() == 0 )
    {
        num_write = socket_.write(msg.data(), num_total);
        if ( num_write > 0 )
        {
            num_remain -= num_write;
            if ( num_remain > 0 )
            {
                TRACE << "need to write more for " << poll_hd_->fd();
            }
        }
        else
        {
            // when directly write falied
            num_write  = 0;
            num_remain = num_total;
            if ( errno != EWOULDBLOCK )
            {
                ERR << "write to " << poll_hd_->fd() << " with " << errno;
            }
        }
    }

    assert(num_remain <= num_total);
    if ( num_remain > 0 )
    {
        size_t num_current_buffer = oup_buffer_.readable_bytes();
        size_t num_after_append   = num_current_buffer + num_remain;
        if ( high_watermark_cb_
             && num_after_append > high_watermark_
             && num_current_buffer < high_watermark_ )
        {
            high_watermark_cb_(shared_from_this(), num_current_buffer);
        }

        oup_buffer_.append(msg.data() + num_write, num_remain);
        if ( !poll_hd_->is_writing() )
        {
            poll_hd_->enable_writing();
        }
    }
    else if ( write_complete_cb_ )
    {
        loop_.queue_in_loop(
            [&] { write_complete_cb_(shared_from_this()); });
    }
}

void tcp_connection::shotdown_in_loop()
{
    loop_.assert_in_loop_thread();

    if ( !poll_hd_->is_writing() )
    {
        socket_.shutdown_write();
    }
}

} // namespace m
