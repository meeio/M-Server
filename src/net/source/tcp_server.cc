#include "tcp_server.hh"

#include <memory>

#include "event_loop.hh"
#include "logger.hh"
#include "tcp_connection.hh"
#include "uninet.hh"

using namespace std::placeholders;

namespace m
{

tcp_server::tcp_server(event_loop& loop, const inet_address& listen_addr)
    : loop_(loop)
    , name_(listen_addr.adress())
    , loop_thread_pool_(loop_)
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

void tcp_server::start()
{
    if ( !started_ )
    {
        loop_thread_pool_.star();
        started_ = true;
    }

    if ( !acceptor_.is_listening() )
    {
        loop_.run_in_loop([&] { acceptor_.listen(); });
    }
}

void tcp_server::register_connection(int sockfd, const inet_address& peer_addr)
{
    std::string  conn_name = name_ + fmt::format("#{:0>5}", next_conn_id_++);
    inet_address host_addr(uni_addr::construct_sockaddr_in(sockfd));

    event_loop& tcp_owner = loop_thread_pool_.get_next_loop();

    // create and save a connection
    tcp_connection_ptr ptcp_coon = std::make_shared<tcp_connection>(
        tcp_owner, conn_name, sockfd, host_addr, peer_addr);

    connections_map_[conn_name] = ptcp_coon;

    // set connection callbacks
    ptcp_coon->set_connection_callback(connection_cb_);
    ptcp_coon->set_message_callback(message_cb_);
    ptcp_coon->set_close_callback(
        [&](auto&&... args) { remove_connection(args...); });

    TRACE << "tcp_connection: " << ptcp_coon->to_string()
          << " created in " << tcp_owner.to_string();

    tcp_owner.run_in_loop(
        [ptcp_coon] { ptcp_coon->connection_estabalished(); });
}

void tcp_server::remove_connection(const tcp_connection_ptr& p_conn)
{
    loop_.run_in_loop(
        [&, p_conn]() { remove_connection_in_loop(p_conn); });
}

void tcp_server::remove_connection_in_loop(const tcp_connection_ptr& p_conn)
{
    loop_.assert_in_loop_thread();

    TRACE << "tcp_connection: " << p_conn->to_string()
          << " removing";

    size_t n = connections_map_.erase(p_conn->name());
    assert(n == 1);

    // this lambda captures connection_ptr by value
    // by doing this, the lifetime of p_ccnn will be extended
    // at least until connection_destroyed() finished.
    event_loop& owner = p_conn->owner_loop();
    owner.queue_in_loop(
        [p_conn] { p_conn->connection_destroyed(); });
}

} // namespace m
