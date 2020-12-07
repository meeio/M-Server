#include <sys/timerfd.h>
#include <unistd.h>

#include "poll_handle.hh"
#include "event_loop.hh"
#include "logger.hh"

m::event_loop* gloop;
int timerfd;

void timeout(const m::time_point& when)
{
    sleep(3);
    DEBUG << "timeout!" << m::clock::time_point_to_str(when);
    // ::read(timerfd);
    gloop->quit();
}

int main(int, char**)
{
    DEBUG_MODE;

    m::event_loop loop;
    gloop = &loop;
    timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    m::poll_handle ch(*gloop, timerfd);
    ch.set_read_callback(timeout);
    ch.enable_reading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);
}