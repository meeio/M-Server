#include "inet_address.hh"

#include <cstring>
#include <arpa/inet.h>

namespace inet_help
{

const sockaddr& cast2addr(const sockaddr_in& addr_in)
{
    auto ptr = reinterpret_cast<const sockaddr*>(&addr_in);
    return *ptr;
};

const sockaddr_in& cast2addr_in(const sockaddr& addr)
{
    auto ptr = reinterpret_cast<const sockaddr_in*>(&addr);
    return *ptr;
};

    
} // namespace help


namespace m
{


inet_address::inet_address(uint16_t port) 
{
    set_addr_in(port, "0.0.0.0");
}


inet_address::inet_address(std::string ip, uint16_t port) 
{
    set_addr_in(port, ip);
}


inet_address::inet_address(sockaddr_in addr_in) 
    : addr_in_(addr_in)
{
    
}


inet_address::inet_address(sockaddr addr) 
    : addr_in_(inet_help::cast2addr_in(addr))
{

}


const sockaddr_in& inet_address::get_sockaddr_in() const
{
    return addr_in_; 
}


const sockaddr& inet_address::get_sockaddr() const
{
    return inet_help::cast2addr(addr_in_);
}



void inet_address::set_addr_in(uint16_t port, std::string ip) 
{
    memset(&addr_in_, 0, sizeof(addr_in_));
    addr_in_.sin_family = AF_INET;
    addr_in_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_in_.sin_port = htons(port);
}

std::ostream& operator<<(std::ostream& os, const inet_address& ia)
{
    const sockaddr_in& addr = ia.get_sockaddr_in();
    auto ip = inet_ntoa(addr.sin_addr);
    auto port = ntohs(addr.sin_port);
    os << ip << ":" << port;
    return os;
}
    
} // namespace m
