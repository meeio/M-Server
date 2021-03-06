#include "uninet.hh"
#include "netinet/tcp.h"
#include "unistd.h"

namespace m::uni_addr
{

sockaddr_in construct_sockaddr_in(const std::string ip, uint16_t port)
{
    sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof addr_in);

    addr_in.sin_family = AF_INET;
    addr_in.sin_port   = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_in.sin_addr);

    return addr_in;
}

sockaddr_in host_sockaddr_in(int sockfd)
{
    sockaddr  addr     = {0, 0};
    socklen_t addr_len = sizeof addr;

    ::getsockname(sockfd, &addr, &addr_len);

    return cast2addr_in(addr);
}

sockaddr_in peer_sockaddr_in(int sockfd)
{
    sockaddr  addr     = {0, 0};
    socklen_t addr_len = sizeof addr;

    ::getpeername(sockfd, &addr, &addr_len);

    return cast2addr_in(addr);
}

} // namespace m::uni_addr

namespace m::sock_op
{

int connect(int sfd, const sockaddr& addr)
{
    return ::connect(sfd, &addr, sizeof addr);
}

int bind(int sfd, const sockaddr& addr)
{
    int ret = ::bind(sfd, &addr, sizeof(addr));
    if ( ret < 0 )
    {
        ERR << "::bind error " << errno;
    }
    else
    {
        TRACE << "socket #" << sfd << " bind";
    }
    return ret;
}

int listen(int sfd)
{
    int ret = ::listen(sfd, SOMAXCONN);
    if ( ret < 0 )
    {
        ERR << "::listen error";
    }
    else
    {
        TRACE << "socket #" << sfd << " listening.";
    }
    return ret;
}

std::tuple<int, sockaddr> accept(int sfd)
{
    sockaddr  peer_sockaddr;
    socklen_t addrlen = sizeof peer_sockaddr;

    int connfd = ::accept4(sfd, &peer_sockaddr, &addrlen,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);

    if ( connfd < 0 )
    {
        int savedErrno = errno;
        INFO << "Socket::accept";
        switch ( savedErrno )
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

int shutdown_write(int sfd)
{
    int ret = ::shutdown(sfd, SHUT_WR);
    if ( ret < 0 )
    {
        ERR << "error " << sfd;
    }
    return ret;
}

int close(int sfd)
{
    int ret = ::close(sfd);
    if ( ret < 0 )
    {
        ERR << "close " << sfd << " " << errno;
    }
    return ret;
}

void set_tcp_no_delay(int sfd, bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

void set_tcp_keep_alive(int sfd, bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sfd, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof optval);
}

int set_reuseaddr(int sfd, bool on)
{
    int reuse = on ? 1 : 0;
    ::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                 &reuse, static_cast<socklen_t>(sizeof(reuse)));
}

int set_reuseport(int sfd, bool on)
{
    int reuse = on ? 1 : 0;
    ::setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT,
                 &reuse, static_cast<socklen_t>(sizeof(reuse)));
}

int get_error(int sockfd)
{
    int       optval;
    socklen_t optlen = sizeof optval;

    if ( ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0 )
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

} // namespace m::sock_op
