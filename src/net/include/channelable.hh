#ifndef __CHANNELABLE_H__
#define __CHANNELABLE_H__

// #include "event_loop.hh"
#include "channel.hh"
#include "copytype.hh"

namespace m
{

/* ----------------------------------------------------------- */
/*                      CLASS CHANNELABLE                      */
/* ----------------------------------------------------------- */

class event_loop;

class channelable 
    : noncopyable
{
public:
    channelable(event_loop* loop, int fd)
        : channel_(loop, fd)
    {
        channel_.set_read_callback(
            [&](auto&... args) { handle_read(args...); });
    }

    /* ---------------------- EVENT CHANGER ---------------------- */
    void channel_enable_reading()
    {
        channel_.enable_reading();
    }
    void channel_disable_all() { channel_.disable_all(); }
    void unwatch_this_channel();
    // void watch_this_channel() { loop()->remove_channel(&channel_); }

    /* ------------------------- OBSERVER ------------------------ */
    event_loop* loop() { return channel_.owner_loop(); }
    int         fd() { return channel_.fd(); }
    void        assert_in_loop_thread();
    int         events() const { return channel_.events(); }

protected:
    /* ---------------------- EVENT HANDLER ---------------------- */
    virtual void handle_read(const time_point_t& when){};
    virtual void handle_write(const time_point_t& when){};
    virtual void handle_close(const time_point_t& when){};
    virtual void handle_error(const time_point_t& when){};

private:
    channel channel_;
};

} // namespace m

#endif // __CHANNELABLE_H__