#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <netinet/in.h>


#define LOCAL "0.0.0.0"

namespace m
{
    
class socket;

class inet_address
{
public:
    /// constructs from ip adress information 
    inet_address(std::string ip, uint16_t port);
    /// consturcts from socket information 
    inet_address(socket);
    inet_address(sockaddr_in);
    inet_address(sockaddr);

    const sockaddr_in& get_sockaddr_in() const;
    const sockaddr&    get_sockaddr() const;

    uint16_t    port() const;
    std::string ip() const;
    std::string adress() const { return ip() + ":" + std::to_string(port()); }

private:
    void set_addr_in(uint16_t port, std::string ip);

    sockaddr_in addr_in_;
};

std::ostream& operator<<(std::ostream&, const inet_address& ia);

} // namespace m

#endif // __INET_ADDRESS_H__