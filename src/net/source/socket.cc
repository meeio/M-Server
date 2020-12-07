#include "socket.hh"

#include "inet_address.hh"
#include "logger.hh"
#include "sys_fd.hh"
#include "uninet.hh"

namespace m
{

// socket::socket()
//     : socket(fd::create_tcp_socket_fd())
// {
//     INFO << socket_fd_ << " is created !";
// }

/* ----------------------------------------------------------- */
/*                         CONSTRUCTORS                        */
/* ----------------------------------------------------------- */

socket::socket()
    : socket_fd_(fd::create_tcp_socket_fd())
{
}

socket::socket(int fd)
    : socket_fd_(fd)
{
}

socket::socket(socket&& rs)
    : socket_fd_(rs.socket_fd_)
{
    rs.socket_fd_ = INVAL_FD;
}

socket::~socket()
{
    if ( socket_fd_ != INVAL_FD )
    {
        sock_op::close(socket_fd_);
        TRACE << "socket " << socket_fd_ << " closed.";
    }
}

/* ----------------------------------------------------------- */
/*                          MODIFIERS                          */
/* ----------------------------------------------------------- */

int socket::bind(const inet_address& addr)
{
    return sock_op::bind(socket_fd_, addr.get_sockaddr());
}

int socket::connect(const inet_address& addr)
{
    int ret = sock_op::connect(socket_fd_, addr.get_sockaddr());
    return ret < 0 ? errno : 0;
}

int socket::listen()
{
    return sock_op::listen(socket_fd_);
}

std::tuple<socket, inet_address> socket::accept()
{
    auto [connfd, peer_sockaddr] = sock_op::accept(socket_fd_);
    inet_address peer_addr(peer_sockaddr);
    socket       sock(connfd);
    return {std::move(sock), peer_addr};
}

ssize_t socket::write(const char* data, const ssize_t len)
{
    int ret = ::write(socket_fd_, data, len);
    return ret;
}

void socket::shutdown_write()
{
    sock_op::shutdown_write(socket_fd_);
}

int socket::get_error()
{
    return sock_op::get_error(socket_fd_);
}

ssize_t socket::readv(iovec* vec, int count)
{
    return ::readv(socket_fd_, vec, count);
}

} // namespace m
