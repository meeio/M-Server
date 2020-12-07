#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <netinet/in.h>

#include "logger.hh"

namespace m::fd
{

inline int create_fd()
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

inline int create_tcp_socket_fd()
{
    int socket_fd = ::socket(AF_INET,
                             SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                             IPPROTO_TCP);
    if (socket_fd < 0)
        ERR << "socket_fd created falied.";

    return socket_fd;
}

inline void none_reaturn_read(int fd)
{
    uint64_t howmany;
    ::read(fd, &howmany, sizeof howmany);
}

itimerspec reset_timerfd_time(int fd, int sec, long nano_sec);

} // namespace m::fd
