
#include "event_loop.hh"
#include "logger.hh"
#include "thread.hh"

#include <unistd.h>

m::event_loop* gloop;

void loop_a_event()
{
    m::event_loop loop;
    loop.loop();
}

int main(int, char const **)
{
    DEBUG_MODE;

    m::event_loop main_loop;

    m::thread loop_thrad(loop_a_event);
    loop_thrad.star();

    main_loop.loop();

    loop_thrad.join();

    return 0;
}
