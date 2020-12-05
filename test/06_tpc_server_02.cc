#include "clock.hh"
#include "event_loop.hh"
#include "inet_address.hh"
#include "logger.hh"
#include "tcp_server.hh"

void on_connection(const m::tcp_connection_ptr& p_conn)
{
    if ( p_conn->connected() )
    {
        DEBUG << "on_connection : connection";

        p_conn->send("AAAAAAAAAAAAA");
        DEBUG << "after AAAA";
        p_conn->send("BBBBBBBBBBBBB");
        DEBUG << "after BBBB";
        p_conn->shotdown();
    }
    else
    {
        DEBUG << "on_connection: disconnection.";
    }
}

void on_message(const m::tcp_connection_ptr& p_conn,
                m::buffer&                     buf,
                m::time_point                revice_time)
{
    DEBUG << "recive " << buf.readable_bytes() << " btyes "
          << "from " << p_conn->name()
          << " at " << m::clock::time_point_to_str(revice_time);
}

int main(int argc, char const* argv[])
{
    TRACE_MODE;

    m::event_loop   loop;
    m::inet_address listen_addr(LOCAL, 11222);
    m::tcp_server   server(loop, listen_addr);

    server.set_connection_callback(on_connection);
    server.set_message_callback(on_message);
    server.set_thread_num(5);
    server.start();

    loop.loop();
    return 0;
}