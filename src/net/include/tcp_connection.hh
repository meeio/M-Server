#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include <functional>
#include <memory>
#include <string>

#include "buffer.hh"
#include "inet_address.hh"
#include "logger.hh"
#include "poll_handler.hh"
#include "socket.hh"

namespace m
{

/* ----------------------------------------------------------- */
/*                       CALLBACK TYPEDEF                      */
/* ----------------------------------------------------------- */

class tcp_connection;

typedef std::shared_ptr<tcp_connection> tcp_connection_ptr;

namespace tcp_cb
{

typedef std::function<void(const tcp_connection_ptr&)>
    connection_callback;

typedef std::function<
    void(const tcp_connection_ptr&, buffer&, const time_point&)>
    message_callback;

typedef std::function<void(const tcp_connection_ptr&)>
    close_callback;

typedef std::function<void(const tcp_connection_ptr&)>
    write_complete_callback;

typedef std::function<void(const tcp_connection_ptr&, size_t water_mark)>
    high_watermark_cb;

} // namespace tcp_cb

/* ----------------------------------------------------------- */
/*                     TCP_CONNECTION CLASS                    */
/* ----------------------------------------------------------- */

class poll_handle;
class event_loop;

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
    , public poll_handler
{
public:
    /* ----------------------- CONSTRUCTORS ---------------------- */

    tcp_connection(event_loop&         loop,
                   const std::string&  name,
                   socket&&            sock,
                   const inet_address& peer_addr);

    ~tcp_connection();

    /* ------------------------- OBSERVER ------------------------ */

    const std::string to_string() const
    {
        return fmt::format(
            "[{}:{}->{}]", name_, socket_.fd(), peer_addr_.adress());
    }

    event_loop& loop() { return loop_; }

    /* ----------------------- CB MODIFIERS ---------------------- */

    void set_connection_callback(tcp_cb::connection_callback cb)
    {
        connection_cb_ = cb;
    }

    void set_message_callback(tcp_cb::message_callback cb)
    {
        message_cb_ = cb;
    }

    void set_close_callback(tcp_cb::close_callback cb)
    {
        close_cb_ = cb;
    }

    void set_write_complete_callback(tcp_cb::write_complete_callback cb)
    {
        write_complete_cb_ = cb;
    }

    void set_high_watermark_callback(tcp_cb::high_watermark_cb cb)
    {
        high_watermark_cb_ = cb;
    }

    /* --------------------- STATE INFOMATION -------------------- */

    std::string  name() const { return name_; }
    inet_address host_addr() const { return host_addr_; }
    inet_address peer_addr() const { return peer_addr_; }

    bool connected() const { return state_ == state_e::connected; }

    /* ------------------------ SOCKET OP ------------------------ */

    /// sending msg to peer, the send opreation will be handled
    /// by the owner loop. If the msg cannot be send in one time,
    /// the remaining msg will be stroed in buffer, and the writeable
    /// event will be listened by the poller.
    /// * The callback will be trigged in function: write_complete_cb_,
    /// * and high_watermark_cb_.
    /// * The callback may be trigged outside function: handle_write.
    void send(const std::string& msg, const size_t len = 0);

    /// set current state to 'shutdowing' and try to shutdown in the
    /// owner loop. If current socket is writing, i.e., requiring write
    /// event, the connection will not be shutdown.
    void shotdown();

    /* ------------------- STATE CHANGING CB ------------------- */

    void connection_estabalished(); // call only once
    void connection_destroyed();    // call only once


    /* ---------------------- EVENT HANDLER ---------------------- */

    virtual void handle_read(const time_point& when) override;
    virtual void handle_write(const time_point& when) override;
    virtual void handle_close(const time_point& when) override;
    virtual void handle_error(const time_point& when) override;

private:
    enum class state_e
    {
        connecting,
        connected,
        disconnecting,
        disconnected,
    };

    void set_state(state_e s) { state_ = s; }

    void send_in_loop(const std::string&, const size_t);
    void shotdown_in_loop();

    /* ------------------------ ATTRIBUTES ----------------------- */

    std::string name_;
    state_e     state_;

    socket socket_;
    buffer inp_buffer_;
    buffer oup_buffer_;
    size_t high_watermark_;

    inet_address host_addr_;
    inet_address peer_addr_;

    /// callbacks
    tcp_cb::connection_callback     connection_cb_;
    tcp_cb::message_callback        message_cb_;
    tcp_cb::close_callback          close_cb_;
    tcp_cb::write_complete_callback write_complete_cb_;
    tcp_cb::high_watermark_cb       high_watermark_cb_;

    event_loop&  loop_;
    poll_handle& poll_hd_;
};

} // namespace m

#endif // !__TCP_CONNECTION_H__