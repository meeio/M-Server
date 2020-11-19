#include "uninet.hh"

namespace m::uni_addr
{
    
sockaddr_in construct_sockaddr_in(const std::string ip, uint16_t port)
{
    sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof addr_in);
    
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_in.sin_addr);

    return addr_in;
}

sockaddr_in construct_sockaddr_in(int sockfd)
{
    sockaddr addr;
    socklen_t addr_len = sizeof addr;
    memset(&addr, 0, addr_len);

    ::getsockname(sockfd, &addr, &addr_len);

    return cast2addr_in(addr);
}

} // namespace m::uni_addr


namespace m::sock_op
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
};

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
};


std::tuple<int, sockaddr> accept(int sfd)
{
    sockaddr  peer_sockaddr;
    socklen_t addrlen = sizeof peer_sockaddr;

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
}; 

int get_error(int sockfd)
{
    int optval;
    socklen_t optlen = sizeof optval;

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) <  0)
    {
        return errno;
    }
    else
    {
        return optval;
    }      
}

} // namespace m::sock_op
