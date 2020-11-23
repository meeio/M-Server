#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"
#include "sys_fd.hh"

#define THREAD_SAFE loop_->assert_in_loop_thread();

namespace m
{

acceptor::acceptor(event_loop* loop, const inet_address& listen_addr)
    : channelable(loop, fd::create_tcp_socket_fd())
    , accept_socket_(fd())
    , is_listing(false)
{
    accept_socket_.bind(listen_addr);
}

void acceptor::listen()
{
    assert_in_loop_thread();

    is_listing = true;
    accept_socket_.listen();
    channel_enable_reading();
}

void acceptor::handle_read(const time_point_t&)
{
    assert_in_loop_thread();

    TRACE << "handle new conn";

    auto [host_fd, peer_addr] = accept_socket_.accept();

    if (host_fd >= 0)
    {

        if (new_conn_cb_)
        {
            new_conn_cb_(host_fd, peer_addr);
        }
        else
        {
            ERR << "no cb is set";
        }
    }
    else
    {
        ERR << "hosf_fd ERROR";
    }
}

void acceptor::set_new_conn_callback(new_coon_callback_t cb)
{
    new_conn_cb_ = cb;
}

} // namespace m
