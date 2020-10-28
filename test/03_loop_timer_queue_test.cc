// copied from m/net/tests/TimerQueue_unittest.cc

#include "event_loop.hh"

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
    printf("msg %s\n", msg);
    if (++cnt == 20)
    {
        g_loop->quit();
    }
}

int main()
{
    using namespace std::placeholders;
    print_tid();
    m::event_loop loop;
    g_loop = &loop;

    loop.run_after(std::bind(print, "once1"), 1000);
    loop.run_after(std::bind(print, "once1.5"), 1500);
    // loop.run_after(2.5, std::bind(print, "once2.5"));
    // loop.run_after(3.5, std::bind(print, "once3.5"));
    // loop.run_every(2, std::bind(print, "every2"));
    // loop.run_every(3, std::bind(print, "every3"));

    loop.loop();
    print("main loop exits");
    sleep(1);
}
