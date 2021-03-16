#include "connector.hh"
#include "event_loop.hh"
#include "logger.hh"
#include "tcp_client.hh"

#include <stdio.h>

void on_message(const m::tcp_connection_ptr& conn,
                m::buffer& buf, const m::time_point&)
{
    std::string msg = buf.retrieve_as_string();
    DEBUG << msg;
    conn->send(msg);
}

int main(int, char**)
{
    DEBUG_MODE;

    DEBUG << "TEST";

    m::event_loop loop;
    m::inet_address addr("127.0.0.1", 9987);

    m::tcp_client client(loop, addr);
    client.set_message_callback(on_message);
    client.connect();

    loop.loop();
}