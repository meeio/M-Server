#include <unistd.h>

#include "logger.hh"
#include "loop_thread.hh"


void run_in_thread()
{
    DEBUG << "run_in_thread(): pid = " << getpid()
          << ", tid = " << m::current_thread::tid();
}

int main()
{
    TRACE_MODE;

    run_in_thread();

    m::loop_thread lp_thread;
    m::event_loop* ploop = lp_thread.start_loop();

    ploop->run_in_loop(run_in_thread);
    sleep(1);

    ploop->run_after(run_in_thread, 2000);
    sleep(10000);

    ploop->quit();

}
