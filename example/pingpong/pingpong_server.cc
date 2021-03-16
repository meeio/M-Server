#include "event_loop.hh"
#include "tcp_server.hh"

void on_connection(const m::tcp_connection_ptr& conn)
{
    if ( conn->connected() )
    {
        conn->set_tcp_no_delay(true);
        conn->send("hello!!!");
    }
}

void on_message(const m::tcp_connection_ptr& conn,
                m::buffer& buf, const m::time_point&)
{
    conn->send(buf.retrieve_as_string());
}

int main()
{
    TRACE_MODE;
    
    int num_thred = 4;

    m::event_loop   loop;
    m::inet_address addr(LOCAL, 9987);
    m::tcp_server   server(loop, addr);

    server.set_connection_callback(on_connection);
    server.set_message_callback(on_message);

    server.set_thread_num(num_thred);

    server.start();

    loop.loop();

    return 0;
}
