#include "inet_address.hh"
#include "logger.hh"
#include "tcp_server.hh"
#include "event_loop.hh"


void on_connection(const m::tcp_connection_ptr& p_conn)
{
    if (p_conn->connected())
    {
        DEBUG << "from " << p_conn->host_addr()
              << " to " << p_conn->peer_addr();
    }
    else
    {
        DEBUG << p_conn->name() << " is down.";
    }
}

void on_message(const m::tcp_connection_ptr& p_conn,
                m::buffer& buf, 
                m::time_point)
{
    DEBUG << "recive " << buf.readable_bytes() << " btyes "
          << "from " << p_conn->name();
}

int main(int, char const**)
{
    TRACE_MODE;

    m::event_loop   loop;
    m::inet_address listen_addr(LOCAL, 11222);
    m::tcp_server   server(loop, listen_addr);

    server.set_connection_callback(on_connection);
    server.set_message_callback(on_message);
    server.start();

    loop.loop();
    return 0;
}
