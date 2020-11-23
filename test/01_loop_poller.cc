#include <sys/timerfd.h>
#include <unistd.h>

#include "channel.hh"
#include "event_loop.hh"
#include "logger.hh"

m::event_loop* gloop;
int timerfd;

void timeout(const m::time_point_t& when)
{
    sleep(3);
    DEBUG << "timeout!" << m::clock::time_point_to_str(when);
    // ::read(timerfd);
    gloop->quit();
}

int main(int argc, char const* argv[])
{
    DEBUG_MODE;

    m::event_loop loop;
    gloop = &loop;
    timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    m::channel ch(gloop, timerfd);
    ch.set_read_callback(timeout);
    ch.enable_reading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);
}