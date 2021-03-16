#include "logger.hh"
#include "time_server.hh"
#include "event_loop.hh"

int main()
{
    TRACE_MODE;

    m::inet_address addr(LOCAL, 9987);
    m::event_loop loop;
    time_server server(loop, addr);
    server.star();
    loop.loop();
}
