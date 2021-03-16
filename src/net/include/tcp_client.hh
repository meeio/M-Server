#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "copytype.hh"

#include "connector.hh"
#include "tcp_connection.hh"

namespace m
{

class event_loop;
class socket;

class tcp_client 
    : public noncopyable
{

public:

    tcp_client(event_loop&, const inet_address);

    void connect()
    {
        connector_.connect();
    }

    void disconnect();
    void stop();

    void handle_new_sock(socket);

    void set_connection_callback(tcp_cb::connection_callback cb)
    {
        conn_cb_ = cb;
    }

    void set_message_callback(tcp_cb::message_callback cb)
    {
        mess_cb_ = cb;
    }

    void set_write_complete_callback(tcp_cb::write_complete_callback cb)
    {
        write_cb_ = cb;
    }

    void close_connection(tcp_connection_ptr);

private:
    event_loop& owner_loop_;
    connector connector_;


    tcp_cb::connection_callback     conn_cb_;
    tcp_cb::message_callback        mess_cb_;
    tcp_cb::write_complete_callback write_cb_;

    int connection_idx_;
    tcp_connection_ptr connection_;

};

} // namespace m

#endif // __TCP_CLIENT_H__