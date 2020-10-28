#ifndef __H_CHANNEL__
#define __H_CHANNEL__

#include <functional>


namespace m
{

class event_loop;

/**
 *  @channel object belongs to one @event_loop
 *  @channel object handle one fd by distributing it's callback function.
 **/
class channel
{
public:
    typedef std::function<void()> t_callback;

    channel(event_loop* loop, int fd);

    void handle_event();

    int         fd() const { return fd_; }
    int         index() { return index_; }
    void        set_index(int idx) { index_ = idx; }
    event_loop* owner_loop() const { return owner_loop_; }

    void set_revents(int revents) { revents_ = revents; }
    int  events() const { return events_; }
    bool is_none_event() const { return events_ == k_none_event; }

    void set_read_callback(t_callback cb) { read_callback = cb; }
    void set_write_callback(t_callback cb) { write_callback = cb; }
    void set_err_callback(t_callback cb) { err_callback = cb; }

    void enable_reading()
    {
        events_ |= k_read_evnet;
        update();
    }

private:
    void update();

    static const int k_none_event;
    static const int k_read_evnet;
    static const int k_write_event;

    event_loop* owner_loop_;
    const int   fd_;
    int         events_;
    int         revents_;
    int         index_;

    t_callback read_callback;
    t_callback write_callback;
    t_callback err_callback;
};

} // namespace m

#endif // !__H_CHANNEL__