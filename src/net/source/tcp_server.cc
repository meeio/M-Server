#include "tcp_server.hh"

#include <memory>

#include "event_loop.hh"
#include "logger.hh"
#include "tcp_connection.hh"

using namespace std::placeholders;

namespace m
{

tcp_server::tcp_server(event_loop* loop, const inet_address& listen_addr)
    : loop_(loop)
    , name_(listen_addr.adress())
    , acceptor_(loop_, listen_addr)
    , next_conn_id_(1)
    , started_(false)
{
    acceptor_.set_new_conn_callback(
        [&](auto&&... args) { return register_connection(args...); });
}

tcp_server::~tcp_server()
{
}

void tcp_server::register_connection(int sockfd, const inet_address& peer_addr)
{
    std::string  conn_name = name_ + fmt::format("#{:0>8}", next_conn_id_++);
    socket       host_socket(sockfd);
    inet_address host_addr(host_socket);

    // create a connection
    tcp_connection_ptr_t ptcp_coon = std::make_shared<tcp_connection>(
        loop_, conn_name, host_socket, host_addr, peer_addr);

    // svae current connectino ptr to server object
    connections_map_[conn_name] = ptcp_coon;

    // set connection callbacks
    ptcp_coon->set_connection_callback(connection_cb_);
    ptcp_coon->set_message_callback(message_cb_);
    ptcp_coon->set_close_callback(
        [&](auto&&... args) { remove_connection(args...); });

    // establish connection to peer
    ptcp_coon->connection_estabalished();
}

void tcp_server::remove_connection(const tcp_connection_ptr_t& p_conn)
{
    loop_->assert_in_loop_thread();
    TRACE << "tcp_server::remove_connection [" << name_ << "] ";

    size_t n = connections_map_.erase(p_conn->name());
    assert(n == 1);

    // this lambda captures connection_ptr by value
    // by doing this, the lifetime of p_ccnn will be extended
    // at least until connection_destroyed() finished.
    loop_->queue_in_loop(
        [p_conn] { p_conn->connection_destroyed(); });
    
}

void tcp_server::start()
{
    if (!acceptor_.is_listening())
    {
        loop_->run_in_loop([&] { acceptor_.listen(); });
    }

    if (!started_)
    {
        started_ = true;
    }
}

} // namespace m
