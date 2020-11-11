#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include <cstdint>
#include <string>
#include <tuple>
#include <iostream>

#include <netinet/in.h>

namespace m
{

class inet_address
{
public:
    inet_address(uint16_t port);

    inet_address(std::string ip, uint16_t port);

    inet_address(sockaddr_in);

    inet_address(sockaddr);

    const sockaddr_in& get_sockaddr_in() const;
    const sockaddr& get_sockaddr() const;

private:
    void set_addr_in(uint16_t port, std::string ip);

    sockaddr_in addr_in_;
};


std::ostream& operator<<(std::ostream&, const inet_address& ia);

} // namespace m

#endif // __INET_ADDRESS_H__