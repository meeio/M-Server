// copied from m/net/tests/TimerQueue_unittest.cc

#include "event_loop.hh"
#include "logger.hh"
#include "timer.hh"

#include <functional>
#include <stdio.h>
#include <unistd.h>

int            cnt = 0;
m::event_loop* g_loop;

void print_tid()
{
    printf("pid = %d, tid = %d\n", getpid(), m::current_thread::tid());
}

void print(const char* msg)
{
    INFO << msg;
    if ( ++cnt == 20 )
    {
        g_loop->quit();
    }
}

int main()
{

    DEBUG_MODE;

    print_tid();

    m::event_loop loop;
    g_loop = &loop;

    auto t0 = loop.run_after(std::bind(print, "once1"), 0);
    DEBUG << &t0;
    // loop.run_after(std::bind(print, "once1.5"), 1500);
    auto t1 = loop.run_every(std::bind(print, "every2.5"), 5500);
    DEBUG << &t1;

    // loop.run_after(3.5, std::bind(print, "once3.5"));
    auto t2 = loop.run_every(std::bind(print, "every2"), 7700);
    DEBUG << &t2;

    // loop.run_every(3, std::bind(print, "every3"));

    loop.loop();
    print("main loop exits");
    sleep(1);
}
