#include "socket.hh"

#include "inet_address.hh"
#include "logger.hh"
#include "sys_fd.hh"

namespace sock_help
{

void bind(int sfd, const sockaddr& addr)
{
    int ret = ::bind(sfd, &addr, sizeof(addr));
    if (ret < 0)
    {
        ERR << "::bind error";
    }
}

void listen(int sfd)
{
    int ret = ::listen(sfd, SOMAXCONN);
    if (ret < 0)
    {
        ERR << "::listen error";
    }
}

std::tuple<sockaddr, int> accept(int sfd)
{
    sockaddr  addr;
    socklen_t addrlen = sizeof(addr);

    int connfd = ::accept4(sfd, &addr, &addrlen,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (connfd < 0)
    {
        int savedErrno = errno;
        INFO << "Socket::accept";
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: // ???
        case EPERM:
        case EMFILE: // per-process lmit of open file desctiptor ???
            // expected errors
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            // unexpected errors
            ERR << "unexpected error of ::accept " << savedErrno;
            break;
        default:
            ERR << "unknown error of ::accept " << savedErrno;
            break;
        }

    }

    return {addr, connfd};
} 

} // namespace help

namespace m
{

socket::socket()
    : socket_fd_(fd::create_tcp_socket_fd())
{
}

socket::socket(int fd)
    : socket_fd_(fd)
{
}
    
socket::~socket() 
{

}

void socket::bind(const inet_address& addr)
{
    sock_help::bind(socket_fd_, addr.get_sockaddr());
}

void socket::listen()
{
    sock_help::listen(socket_fd_);
}


std::tuple<inet_address, int> socket::accept()
{
    auto [isockaddr, connfd] = sock_help::accept(socket_fd_);
    inet_address idress(isockaddr);
    return {idress, connfd};
}

} // namespace m
