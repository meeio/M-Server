#include "acceptor.hh"
#include "event_loop.hh"
#include "inet_address.hh"
#include "poll_handle.hh"
#include "logger.hh"
#include "sys_fd.hh"

#define THREAD_SAFE loop_->loop_.assert_in_loop_thread();

namespace m
{

acceptor::acceptor(event_loop& loop, const inet_address& listen_addr)
    : accept_socket_()
    , is_listing(false)
    , loop_(loop)
    , poll_hd_(loop_.register_polling(accept_socket_.fd()))
{
    poll_hd_.bind_handler(*this);
    accept_socket_.bind(listen_addr);
}

void acceptor::listen()
{
    loop_.assert_in_loop_thread();

    is_listing = true;
    accept_socket_.listen();
    poll_hd_.enable_reading();
}

void acceptor::handle_read(const time_point&)
{
    loop_.assert_in_loop_thread();

    auto [sock, peer_addr] = accept_socket_.accept();

    if (sock.is_valid())
    {

        if (new_conn_cb_)
        {
            new_conn_cb_(std::move(sock));
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

void acceptor::set_new_sock_callback(new_socket_callback cb)
{
    new_conn_cb_ = cb;
}

} // namespace m
