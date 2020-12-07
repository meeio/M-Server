#include "sys_fd.hh"

namespace m::fd
{

itimerspec reset_timerfd_time(int fd, int sec, long nano_sec)
{
    itimerspec old_time = {
        .it_interval = {0, 0},
        .it_value    = {0, 0}};
        
    itimerspec new_time = {
        .it_interval = {0, 0},
        .it_value    = {
            .tv_sec  = sec,
            .tv_nsec = nano_sec}};

    timerfd_settime(fd, 0, &new_time, &old_time);

    return old_time;
}

} // namespace m::fd
