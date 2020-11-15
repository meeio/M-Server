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
    else
    {
        TRACE << "socket " << sfd << " bind."; 
    }
    
}

void listen(int sfd)
{
    int ret = ::listen(sfd, SOMAXCONN);
    if (ret < 0)
    {
        ERR << "::listen error";
    }
    else
    {
        TRACE << "socket " << sfd << " listening.";
    }
}

std::tuple<int, sockaddr> accept(int sfd)
{
    TRACE << "socket " << sfd << " has a accept";
    sockaddr  peer_sockaddr;
    socklen_t addrlen = sizeof(peer_sockaddr);

    int connfd = ::accept4(sfd, &peer_sockaddr, &addrlen,
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

    return {connfd, peer_sockaddr};
} 

} // namespace help

namespace m
{

socket::socket()
    : socket(fd::create_tcp_socket_fd())
{
}

socket::socket(int fd)
    : socket_fd_(fd)
{
    TRACE << "socket " << fd << " created.";
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


std::tuple<int, inet_address> socket::accept()
{
    auto [connfd, peer_sockaddr] = sock_help::accept(socket_fd_);
    inet_address peer_addr(peer_sockaddr);
    return {connfd, peer_addr};
}

} // namespace m
