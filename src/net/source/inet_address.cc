#include "inet_address.hh"

#include <arpa/inet.h>
#include <cstring>

#include "socket.hh"
#include "uninet.hh"

namespace m
{

inet_address::inet_address(std::string ip, uint16_t port)
    : addr_in_(
          uni_addr::construct_sockaddr_in(ip, port))
{
}

inet_address::inet_address(sockaddr_in addr_in)
    : addr_in_(addr_in)
{
}

inet_address::inet_address(sockaddr addr)
    : addr_in_(uni_addr::cast2addr_in(addr))
{
}

const sockaddr_in& inet_address::get_sockaddr_in() const
{
    return addr_in_;
}

const sockaddr& inet_address::get_sockaddr() const
{
    return uni_addr::cast2addr(addr_in_);
}

std::string inet_address::ip() const
{
    return uni_addr::get_ip(addr_in_);
}

uint16_t inet_address::port() const
{
    return uni_addr::get_port(addr_in_);
}

std::ostream& operator<<(std::ostream& os, const inet_address& ia)
{
    auto ip   = ia.ip();
    auto port = ia.port();
    os << ip << ":" << port;
    return os;
}

} // namespace m
