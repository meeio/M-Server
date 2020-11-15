#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include <memory>
#include <string>

#include "channel.hh"
#include "copytype.hh"
#include "inet_address.hh"
#include "socket.hh"

namespace m
{

class tcp_connection;

typedef std::shared_ptr<tcp_connection> tcp_connection_ptr_t;

namespace tcp_cb
{

typedef std::function<void(const tcp_connection_ptr_t&)> connection_callback_t;

typedef std::function<
    void(const tcp_connection_ptr_t&, const char* data, size_t len)>
    message_callback_t;

} // namespace tcp_cb

class tcp_connection
    : noncopyable
    , std::enable_shared_from_this<tcp_connection>
{
public:
    tcp_connection(event_loop*         loop,
                   const std::string&  name,
                   socket&             socket,
                   const inet_address& host_addr,
                   const inet_address& peer_addr);


    void set_connection_callback(tcp_cb::connection_callback_t cb) 
    {
        connection_cb_ = cb;
    }

    void set_message_callback(tcp_cb::message_callback_t cb)
    {
        message_cb_ = cb;
    }

    event_loop*        loop() { return loop_; }
    const std::string  name() const { return name_; }
    const inet_address host_addr() const { return host_addr_; }
    const inet_address peer_addr() const { return peer_addr_; }
    const bool         connected() const { return state_ == state_e::k_connected; }
    
    void connection_estabalished(); // call only once
    void connection_destroyed();    // call only once

private:
    enum class state_e
    {
        k_connecting,
        k_connected,
        k_disconnected
    };

    void set_state(state_e s)
    {
        state_ = s;
    }

    void handle_read();
    void handle_write();
    void handle_close();
    void handle_error();

    event_loop* loop_;
    std::string name_;
    state_e     state_;

    socket  socket_;
    channel channel_;

    inet_address host_addr_;
    inet_address peer_addr_;

    tcp_cb::connection_callback_t connection_cb_;
    tcp_cb::message_callback_t    message_cb_;
};

} // namespace m

#endif // !__TCP_CONNECTION_H__