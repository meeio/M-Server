#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "copytype.hh"
#include "poll_handler.hh"
#include "socket.hh"

namespace m
{

class event_loop;
class poll_handle;
class inet_address;

class acceptor
    : public poll_handler
    , public noncopyable
{
public:
    typedef std::function<void(socket)> new_socket_callback;

    acceptor(event_loop&, const inet_address&);

    void set_new_sock_callback(new_socket_callback);

    void listen();
    bool is_listening() { return is_listing; }

protected:
    virtual void handle_read(const time_point&) override;

private:
    new_socket_callback new_conn_cb_;
    socket            accept_socket_;
    bool              is_listing;

    event_loop&  loop_;
    poll_handle& poll_hd_;
};

} // namespace m

#endif // __ACCEPTOR_H__