#ifndef __H_CHANNEL__
#define __H_CHANNEL__

#include <functional>

#include "clock.hh"
// #include "event_loop.hh"

namespace m
{

class event_loop;

/**
 * * channel object belongs to one event_loop.
 * * channel object handle one fd by distributing it's callback function.
 **/
class channel
{
public:
    typedef std::function<void()>                    callback_t;
    typedef std::function<void(const time_point_t&)> reade_callback_t;

    channel(event_loop* loop, int fd);
    ~channel();

    /* ------------------------- OBSERVER ------------------------ */

    event_loop* owner_loop() const { return owner_loop_; }
    int         fd() const { return fd_; }
    int         events() const { return events_; }
    bool        is_none_event() const { return events_ == k_none_event; }

    /* ---------------------- EVENT MODIFERS --------------------- */

    void handle_event(const time_point_t& recive_time);
    /// modifers for revents
    void set_revents(int revents) { revents_ = revents; }
    /// modifers for events
    void enable_reading();
    void disable_all();

    /* -------------------- CALLBACK MODIFIERS ------------------- */

    void set_read_callback(reade_callback_t cb) { read_callback_ = cb; }
    void set_write_callback(callback_t cb) { write_callback_ = cb; }
    void set_err_callback(callback_t cb) { err_callback_ = cb; }
    void set_close_callback(callback_t cb) { close_callback_ = cb; }

    /* ------------------------ FOR POLLER ----------------------- */
    int  index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    static const int k_none_event;
    static const int k_read_evnet;
    static const int k_write_event;
    
private:
    void update();


    event_loop* owner_loop_;
    const int   fd_;
    int         events_;
    int         revents_;
    int         index_;
    bool        handling_event_;

    reade_callback_t read_callback_;
    callback_t       write_callback_;
    callback_t       err_callback_;
    callback_t       close_callback_;
};

} // namespace m

#endif // !__H_CHANNEL__