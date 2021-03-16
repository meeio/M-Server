#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "inet_address.hh"
#include "socket.hh"
#include "timer_queue.hh"
#include "poll_handler.hh"

namespace m
{

class event_loop;

class connector
    : public poll_handler
{
public:
    typedef std::function<void(socket)> new_connection_cb;

    const static int RETRY_DELAY_MS = 500;
    const static int MAX_RETRY_DELAY_MS = 30000;

    connector(event_loop&, inet_address);
    ~connector();

    /* ------------------------ CONNECTORS ----------------------- */
    void connect();
    void reconnect();
    void stop();

    void set_new_socket_cb(new_connection_cb cb_)
    {
        new_connection_cb_ = cb_;
    }

public:
    /* ---------------------- EVENT HANDLERS --------------------- */

    virtual void handle_write(const time_point&) override;
    virtual void handle_error(const time_point&) override;

private:
    /* ----------------------- INNER STATE ----------------------- */

    enum class state
    {
        stop,
        connected,
        connecting,
        disconnected
    };

    void set_state(state s) { state_ = s; }

    /* -------------------- INLOOP CONNECTORS -------------------- */
    void wait_writable();

    void connect_in_loop();
    void retry_connect();
    void reconnect_in_loop();
    void disconnect_in_loop();

    /* ------------------------ ATTRIBUTES ----------------------- */

    event_loop&             loop_;
    poll_handle*            poll_hd_;

    state                   state_;
    std::unique_ptr<socket> psocket_;
    inet_address            peer_addr_;
    int                     retry_delay_ms;
    timer_handle*           retry_timer_;

    new_connection_cb new_connection_cb_;
};

} // namespace m

#endif // __CONNECTOR_H__s