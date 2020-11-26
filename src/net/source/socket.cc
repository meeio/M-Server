#include "socket.hh"

#include "inet_address.hh"
#include "logger.hh"
#include "sys_fd.hh"
#include "uninet.hh"

namespace m
{

socket::socket()
    : socket(fd::create_tcp_socket_fd())
{
}

socket::socket(int fd)
    : socket_fd_(fd)
{
}
    
socket::~socket() 
{
    sock_op::close(socket_fd_);
    TRACE << "socket " << socket_fd_ << " closed.";
}

void socket::bind(const inet_address& addr)
{
    sock_op::bind(socket_fd_, addr.get_sockaddr());
}

void socket::listen()
{
    sock_op::listen(socket_fd_);
}

std::tuple<int, inet_address> socket::accept()
{
    auto [connfd, peer_sockaddr] = sock_op::accept(socket_fd_);
    inet_address peer_addr(peer_sockaddr);
    return {connfd, peer_addr};
}



ssize_t socket::write(const char* data, const ssize_t len) 
{
    return ::write(socket_fd_, data, len);
}


void socket::shutdown_write() 
{
    sock_op::shutdown_write(socket_fd_);
}

int socket::get_error() 
{
    return sock_op::get_error(socket_fd_);
}


} // namespace m
