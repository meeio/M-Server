#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <functional>
#include <map>

#include "loop_thread_pool.hh"
#include "acceptor.hh"
#include "tcp_connection.hh"

namespace m
{

class event_loop;
class tcp_connection;

class tcp_server
{
public:

    /* --------------------- (DE)CONSTRUCTOR --------------------- */

    tcp_server(event_loop&, const inet_address&);
    ~tcp_server();

    /* -------------------- CALLBACK MODIFERS -------------------- */

    /// @brief set callback for @c tcp_connection when connection state changes.
    /// @see tcp_connection
    void set_connection_callback(tcp_cb::connection_callback cb)
    {
        connection_cb_ = cb;
    }

    /// @brief set callback for @c tcp_connection when new message revices.
    /// @see tcp_connection
    void set_message_callback(tcp_cb::message_callback cb)
    {
        message_cb_ = cb;
    }

    /* -------------------------- RUNNER ------------------------- */

    /// @brief after starting, server will listen to specific inet_adress.
    /// @see acceptor
    void start();

    /// @brief set thread number of thred_pool
    /// @see loop_thread_pool
    void set_thread_num(size_t n) { loop_thread_pool_.set_thread_num(n); }
    

private:
    typedef std::map<std::string, tcp_connection_ptr> connection_map;

    /// @brief callback functions when socket accptor accept new connections.
    /// @details this function take socket_fd and peer address as input, and 
    /// create new tcp_connection object, and then distritute it into I/O thread.
    /// @see tcp_connection @see loop_thread_pool
    void register_connection(socket, const inet_address&);

    /// @brief thread safe to remove connection
    void remove_connection(const tcp_connection_ptr&);

    /// @brief not-thread-safe but in-loop to remove conneciton
    void remove_connection_in_loop(const tcp_connection_ptr&);

    event_loop&       loop_;
    const std::string name_;
    loop_thread_pool  loop_thread_pool_;

    acceptor          acceptor_;
    connection_map    connections_map_;

    tcp_cb::connection_callback connection_cb_;
    tcp_cb::message_callback    message_cb_;

    bool started_;
    int  next_conn_id_;
};

} // namespace m

#endif // __TCP_SERVER_H__