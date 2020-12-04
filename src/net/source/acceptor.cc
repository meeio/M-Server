#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"
#include "sys_fd.hh"

#define THREAD_SAFE loop_->loop_.assert_in_loop_thread();

namespace m
{

acceptor::acceptor(event_loop& loop, const inet_address& listen_addr)
    : loop_(loop)
    , accept_socket_()
    , pollee_(loop_, this, accept_socket_.fd())
    , is_listing(false)
{
    accept_socket_.bind(listen_addr);
}

void acceptor::listen()
{
    loop_.assert_in_loop_thread();

    is_listing = true;
    accept_socket_.listen();
    pollee_.enable_reading();
}

void acceptor::handle_read(const time_point&)
{
    loop_.assert_in_loop_thread();

    auto [sock, peer_addr] = accept_socket_.accept();

    if (sock.is_valid())
    {

        if (new_conn_cb_)
        {
            new_conn_cb_(std::move(sock), peer_addr);
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

void acceptor::set_new_conn_callback(new_coon_callback cb)
{
    new_conn_cb_ = cb;
}

} // namespace m
