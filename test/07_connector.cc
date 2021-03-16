#include "connector.hh"
#include "event_loop.hh"
#include "logger.hh"

#include <stdio.h>

m::event_loop* g_loop;

void cb(m::socket sock)
{
    INFO << "connect " << sock.fd();
    g_loop->quit();
}

int main(int, char**)
{

    DEBUG_MODE;

    m::event_loop loop;
    g_loop = &loop;
    m::inet_address addr("127.0.0.1", 11222);
    m::connector    aconnector(loop, addr);
    aconnector.set_new_socket_cb(cb);
    aconnector.connect();

    loop.loop();
}