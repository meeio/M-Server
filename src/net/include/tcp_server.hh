#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <functional>
#include <map>

#include "tcp_connection.hh"
#include "acceptor.hh"

namespace m
{

class event_loop;
class tcp_connection;


class tcp_server
{
public:
    tcp_server(event_loop*, const inet_address&);
    ~tcp_server();

    void set_connection_callback(tcp_cb::connection_callback_t cb) 
    { 
        connection_cb_ = cb; 
    }

    void set_message_callback(tcp_cb::message_callback_t cb) 
    { 
        message_cb_ = cb; 
    }

    void start();

private:
    typedef std::map<std::string, tcp_connection_ptr_t> connection_map_t_;

    void register_connection(int, const inet_address&);
    void remove_connection(const tcp_connection_ptr_t&);

    event_loop*       loop_;
    const std::string name_;
    acceptor          acceptor_;
    connection_map_t_ connections_map_;

    tcp_cb::connection_callback_t connection_cb_;
    tcp_cb::message_callback_t    message_cb_;

    bool started_;
    int  next_conn_id_;

};

} // namespace m

#endif // __TCP_SERVER_H__