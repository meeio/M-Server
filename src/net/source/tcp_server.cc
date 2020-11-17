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

    tcp_connection_ptr_t ptcp_coon = std::make_shared<tcp_connection>(
        loop_, conn_name, host_socket, host_addr, peer_addr);

    ptcp_coon->set_connection_callback(connection_cb_);
    ptcp_coon->set_message_callback(message_cb_);

    ptcp_coon->connection_estabalished();

    INFO << "new conn [" << conn_name << "]"
         << "from " << peer_addr.ip() << ":" << peer_addr.port();
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
