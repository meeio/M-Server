#include <unistd.h>
#include <sys/timerfd.h>

#include "logger.hh"

namespace m
{

namespace timerfd
{

int create_timer_fd()
{
    int timer_fd = ::timerfd_create(CLOCK_MONOTONIC,
                                    TFD_NONBLOCK | TFD_CLOEXEC);

    if (timer_fd < 0)
        ERR << "timer_fd created falied.";

    return timer_fd;
}

itimerspec reset_timerfd_time(int fd, int sec, long nano_sec)
{
    itimerspec new_time;
    itimerspec old_time;

    timespec new_time_v = {
        .tv_sec = sec,
        .tv_nsec = nano_sec
    };

    new_time.it_value = new_time_v;

    timerfd_settime(fd, 0, &new_time, &old_time);

    return old_time;
}

}
    
} // namespace m
