#ifndef __H_CHANNEL__
#define __H_CHANNEL__

#include <functional>

#include "clock.hh"
#include "copytype.hh"

namespace m
{

class event_loop;
class poller;
class poll_handler;

/**
 * channel object belongs to one event_loop.
 * channel object handle one fd by distributing it's callback function.
 */
class poll_handle
    : noncopyable
{
public:
    typedef std::function<void(const time_point&)> event_callback;

    static const int EVENT_NONE;
    static const int EVENT_READ;
    static const int EVENT_WRITE;

    /* ----------------------- CONSTRUCTORS ---------------------- */
    poll_handle(event_loop& loop, int fd);
    ~poll_handle();

    /* ------------------------- OBSERVER ------------------------ */

    event_loop& owner_loop() const { return owner_loop_; }

    int  fd() const { return fd_; }
    int  events() const { return events_; }
    bool is_none_event() const { return events_ == EVENT_NONE; }
    bool is_writing() const { return events_ & EVENT_WRITE; }
    bool is_reading() const { return events_ & EVENT_READ; }

    /* ---------------------- EVENT MODIFERS --------------------- */

    void handle_event(const time_point& recive_time);
    void set_revents(int revents) { revents_ = revents; }
    void enable_reading();
    void enable_writing();
    void disable_writing();
    void disable_all();

    /* -------------------- CALLBACK MODIFIERS ------------------- */

    void bind_handler(poll_handler&);
    void set_read_callback(event_callback cb) { read_callback_ = cb; }
    void set_write_callback(event_callback cb) { write_callback_ = cb; }
    void set_err_callback(event_callback cb) { err_callback_ = cb; }
    void set_close_callback(event_callback cb) { close_callback_ = cb; }

    /* ------------------------ FOR POLLER ----------------------- */

    int  index() { return index_; }
    void set_index(int idx) { index_ = idx; }

private:
    void update();

    event_loop& owner_loop_;
    const int   fd_;
    int         index_;

    int  events_;
    int  revents_;
    bool handling_event_;

    event_callback read_callback_;
    event_callback write_callback_;
    event_callback err_callback_;
    event_callback close_callback_;
};

} // namespace m

#endif // !__H_CHANNEL__