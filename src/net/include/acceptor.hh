#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "channel.hh"
#include "pollable.hh"
#include "copytype.hh"
#include "inet_address.hh"
#include "socket.hh"

namespace m
{

class event_loop;

class acceptor
    : public pollable
{
public:
    typedef std::time_point<void(int, const inet_address&)>
        new_coon_callback_t;

    acceptor(event_loop&, const inet_address&);

    void set_new_conn_callback(new_coon_callback_t);

    void listen();
    bool is_listening() { return is_listing; }

protected:
    virtual void handle_read(const time_point&) override;

private:
    void handle_new_conn_();

    new_coon_callback_t new_conn_cb_;
    socket              accept_socket_;

    bool is_listing;
};

} // namespace m

#endif // __ACCEPTOR_H__