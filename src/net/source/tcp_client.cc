#include "tcp_client.hh"

#include "event_loop.hh"
#include "inet_address.hh"
#include "socket.hh"

#include <string>

namespace m
{

tcp_client::tcp_client(event_loop& loop, const inet_address addr)
    : owner_loop_(loop)
    , connector_(loop, addr)
{
    connector_.set_new_socket_cb(
        [&](auto&&... arg) {
            handle_new_sock(std::forward<decltype(arg)>(arg)...);
        });
}

void tcp_client::handle_new_sock(socket sock)
{

    std::string conn_name = fmt::format("#{:0>5}", connection_idx_++);

    tcp_connection_ptr conn_ = std::make_shared<tcp_connection>(
        owner_loop_, conn_name, std::move(sock));

    conn_->set_connection_callback(conn_cb_);
    conn_->set_message_callback(mess_cb_);
    conn_->set_write_complete_callback(write_cb_);
    conn_->set_close_callback(
        [&](auto&&... args) { close_connection(args...); });

    connection_ = conn_;
}

void tcp_client::close_connection(tcp_connection_ptr closed_conn_)
{
    owner_loop_.assert_in_loop_thread();
    assert(closed_conn_ == connection_);
    connection_.reset();
}

} // namespace m