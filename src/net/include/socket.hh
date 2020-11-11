#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <tuple>

namespace m
{

class inet_address;

class socket
{
public:
    socket();
    socket(int);
    ~socket();

    void                          bind(const inet_address&);
    void                          listen();
    std::tuple<inet_address, int> accept();

    inline int fd() { return socket_fd_; }

private:
    const int socket_fd_;
};

} // namespace m

#endif // __SOCKET_H__