#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <tuple>

#include "copytype.hh"

namespace m
{

class inet_address;

class socket : noncopyable
{
public:
    socket();
    socket(int);
    ~socket();

    void                          bind(const inet_address&);
    void                          listen();
    std::tuple<int, inet_address> accept();

    int get_error();

    inline int fd() { return socket_fd_; }

private:
    const int socket_fd_;
};

} // namespace m

#endif // __SOCKET_H__