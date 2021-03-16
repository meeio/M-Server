#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "copytype.hh"
#include "inet_address.hh"
#include "uninet.hh"

#include <sys/types.h>
#include <sys/uio.h>
#include <tuple>

namespace m
{

class inet_address;

class socket : noncopyable
{
public:
    const int INVAL_FD = -1;

    /* ----------------------- CONSTRUCTERS ---------------------- */
    socket();
    socket(int);
    socket(socket&&);
    ~socket();

    /* ------------------------ OBSERVERS ------------------------ */
    bool         is_valid() { return fd() != INVAL_FD; }
    inet_address host_addr();
    inet_address peer_addr();
    inline int   fd() const { return socket_fd_; }
    int          get_error();

    /* ------------------------- MODIFIER ------------------------ */
    int     bind(const inet_address&);
    int     listen();
    int     connect(const inet_address&);
    ssize_t readv(iovec*, int);
    socket  accept();
    ssize_t write(const char*, const ssize_t);
    void    shutdown_write();
    void    set_tcp_no_delay(bool);

private:
    std::unique_ptr<inet_address> host_addrp;
    std::unique_ptr<inet_address> peer_addrp;
    int socket_fd_;
};

} // namespace m

#endif // __SOCKET_H__