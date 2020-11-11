#include "sys_fd.hh"

namespace m::fd
{

itimerspec reset_timerfd_time(int fd, int sec, long nano_sec)
{
    itimerspec new_time;
    itimerspec old_time;

    bzero(&new_time, sizeof new_time);
    bzero(&old_time, sizeof old_time);

    timespec new_time_v = {
        .tv_sec = sec,
        .tv_nsec = nano_sec
    };

    new_time.it_value = new_time_v;

    timerfd_settime(fd, 0, &new_time, &old_time);

    return old_time;
}
    
} // namespace m::fd
