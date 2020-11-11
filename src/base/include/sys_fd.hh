#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>

#include "logger.hh"

namespace m::fd
{

inline int cread_fd()
{
    return ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
}

inline int create_timer_fd()
{
    int timer_fd = ::timerfd_create(CLOCK_MONOTONIC,
                                    TFD_NONBLOCK | TFD_CLOEXEC);

    if (timer_fd < 0)
        ERR << "timer_fd created falied.";

    return timer_fd;
}

inline void read_fd(int fd)
{
    uint64_t howmany;
    ssize_t n = ::read(fd, &howmany, sizeof howmany);
}

itimerspec reset_timerfd_time(int fd, int sec, long nano_sec);

} // namespace m
