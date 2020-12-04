#ifndef __UNINET_H__
#define __UNINET_H__

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <string>

#include "logger.hh"

namespace m::uni_addr
{

/**
 * functions to cast between sockaddr and sockaddr_in.
 **/

inline const sockaddr& cast2addr(const sockaddr_in& addr_in)
{
    auto ptr = reinterpret_cast<const sockaddr*>(&addr_in);
    return *ptr;
};

inline const sockaddr_in& cast2addr_in(const sockaddr& addr)
{
    auto ptr = reinterpret_cast<const sockaddr_in*>(&addr);
    return *ptr;
};

inline sockaddr& cast2addr(sockaddr_in& addr_in)
{
    auto ptr = reinterpret_cast<sockaddr*>(&addr_in);
    return *ptr;
};

inline sockaddr_in& cast2addr_in(sockaddr& addr)
{
    auto ptr = reinterpret_cast<sockaddr_in*>(&addr);
    return *ptr;
};

/**
 * functions to convert ip and port to sockaddr_in, 
 * and vice verse.
 **/

inline const uint16_t get_port(const sockaddr_in addr_in)
{
    return ntohs(addr_in.sin_port);
}

inline const std::string get_ip(const sockaddr_in addr_in)
{
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_in.sin_addr, buf, INET_ADDRSTRLEN);
    return std::string(buf);
}

sockaddr_in construct_sockaddr_in(const std::string ip, uint16_t port);
sockaddr_in construct_sockaddr_in(int sockfd);

} // namespace m::uni_addr

namespace m::sock_op
{

int                       connect(int sfd, const sockaddr& addr);
int                       bind(int sfd, const sockaddr& addr);
int                       listen(int sfd);
std::tuple<int, sockaddr> accept(int sfd);
int                       close(int sfd);
int                       shutdown_write(int sfd);
void                      set_tcp_no_delay(int sfd, bool on);
int                       get_error(int sockfd);

} // namespace m::sock_op

#endif // __UNINET_H__