#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"

#define THREAD_SAFE loop_->assert_in_loop_thread();

namespace m
{

acceptor::acceptor(event_loop* loop, const inet_address& listen_addr)
    : loop_(loop)
    , accept_socket_()
    , accept_channel_(loop, accept_socket_.fd())
    , is_listing(false)
{
    accept_socket_.bind(listen_addr);
    accept_channel_.set_read_callback([&] { handle_new_conn_(); });
}

void acceptor::listen()
{
    loop_->assert_in_loop_thread();

    is_listing = true;
    accept_socket_.listen();
    accept_channel_.enable_reading();
}

void acceptor::handle_new_conn_()
{
    loop_->assert_in_loop_thread();

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
