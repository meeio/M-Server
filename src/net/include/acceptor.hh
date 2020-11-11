#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "channel.hh"
#include "socket.hh"
#include "inet_address.hh"

namespace m
{

class event_loop;

class acceptor
{
public:
    typedef std::function<void (int sockfd, 
                                const inet_address&)> new_coon_callback_t;

    acceptor(event_loop*, const inet_address&);

    void set_new_conn_callback(new_coon_callback_t);

    void listen();

private:
    event_loop* loop_;
    socket accept_socket_;
    channel accept_channel_;

    new_coon_callback_t new_conn_cb_;

    bool is_listing;
};

} // namespace m

#endif // __ACCEPTOR_H__