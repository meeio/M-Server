#include "time_server.hh"
#include "event_loop.hh"

time_server::time_server(m::event_loop& loop, m::inet_address addr)
    : loop_(loop)
    , server_(loop_, addr)
{
    server_.set_connection_callback(
        [&](auto&&... args) { return on_connection(args...); });

}


void time_server::on_connection(const m::tcp_connection_ptr& conn_)
{
    bool connected = conn_->connected();

    INFO << "timeserver - " << conn_->peer_addr().adress()
         << " -> " << conn_->host_addr().adress()
         << " is " << (connected ? "connected" : "disconnected");

    if ( connected )
    {
        m::time_point now  = m::clock::now();
        std::string   time = m::clock::time_point_to_str(now);
        conn_->send(time);
        conn_->shotdown();
    }
}


void time_server::on_message(const m::tcp_connection_ptr& conn,
                    m::buffer& buf, m::time_point& time) 
{
    std::string msg(buf.retrieve_as_string());
    INFO << conn->name() << " discards " << msg.size()
        << " bytes received at " << m::clock::time_point_to_str(time);
}

