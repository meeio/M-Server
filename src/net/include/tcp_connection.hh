#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include <memory>
#include <string>

#include "buffer.hh"
#include "channel.hh"
#include "channelable.hh"
#include "clock.hh"
#include "copytype.hh"
#include "inet_address.hh"
#include "socket.hh"

namespace m
{

/* ----------------------------------------------------------- */
/*                       CALLBACK TYPEDEF                      */
/* ----------------------------------------------------------- */

class tcp_connection;

typedef std::shared_ptr<tcp_connection> tcp_connection_ptr_t;

namespace tcp_cb
{

typedef std::function<void(const tcp_connection_ptr_t&)>
    connection_callback_t;

typedef std::function<
    void(const tcp_connection_ptr_t&, buffer&, const time_point_t&)>
    message_callback_t;

typedef std::function<void(const tcp_connection_ptr_t&)>
    close_callback_t;

} // namespace tcp_cb

/* ----------------------------------------------------------- */
/*                     TCP_CONNECTION CLASS                    */
/* ----------------------------------------------------------- */

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
    , public channelable
{
public:
    tcp_connection(event_loop*         loop,
                   const std::string&  name,
                   socket&             socket,
                   const inet_address& host_addr,
                   const inet_address& peer_addr);

    ~tcp_connection();

    /* ----------------------- CB MODIFIERS ---------------------- */

    void set_connection_callback(tcp_cb::connection_callback_t cb)
    {
        connection_cb_ = cb;
    }

    void set_message_callback(tcp_cb::message_callback_t cb)
    {
        message_cb_ = cb;
    }

    void set_close_callback(tcp_cb::close_callback_t cb)
    {
        close_cb_ = cb;
    }

    /* --------------------- STATE INFOMATION -------------------- */

    const std::string  name() const { return name_; }
    const inet_address host_addr() const { return host_addr_; }
    const inet_address peer_addr() const { return peer_addr_; }
    const bool         connected() const { return state_ == state_e::k_connected; }

    /* ------------------- STATE CHANGING CB ------------------- */

    void connection_estabalished(); // call only once
    void connection_destroyed();    // call only once

protected:
    /* ---------------------- EVENT HANDLER ---------------------- */
    virtual void handle_read(const time_point_t& when) override;
    virtual void handle_write(const time_point_t& when) override;
    virtual void handle_close(const time_point_t& when) override;
    virtual void handle_error(const time_point_t& when) override;

private:
    enum class state_e
    {
        k_connecting,
        k_connected,
        k_disconnected,
    };

    void set_state(state_e s) { state_ = s; }

    std::string name_;
    state_e     state_;

    buffer buffer_;
    socket socket_;

    inet_address host_addr_;
    inet_address peer_addr_;

    tcp_cb::connection_callback_t connection_cb_;
    tcp_cb::message_callback_t    message_cb_;
    tcp_cb::close_callback_t      close_cb_;
};

} // namespace m

#endif // !__TCP_CONNECTION_H__