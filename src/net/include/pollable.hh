#ifndef __CHANNELABLE_H__
#define __CHANNELABLE_H__

#include <memory>

#include "channel.hh"
#include "copytype.hh"

namespace m
{

/* ----------------------------------------------------------- */
/*                        CLASS POLLABLE                       */
/* ----------------------------------------------------------- */

class event_loop;

class pollable
    : noncopyable
{
public:
    const static int EMPTY_FD = -1;

    pollable(event_loop& loop, int fd)
        : owner_loop_(loop)
    {
        if ( fd != pollable::EMPTY_FD )
        {
            bind_this_channel_to_fd(fd);
        }
        else
        {
            pchannel_ = nullptr;
        }
    }

    /* --------------------- CHANNEL MODIFIER -------------------- */
    
    /// @brief thread safe rebind channel to fd.
    void rebind_channel(int);

    /* ---------------------- EVENT CHANGER ---------------------- */
    void channel_enable_reading() { pchannel_->enable_reading(); }
    void channel_enable_writing() { pchannel_->enable_writing(); }

    void channel_disable_writing() { pchannel_->disable_writing(); }
    void channel_disable_all() { pchannel_->disable_all(); }

    void unwatch_this_channel();

    /* ------------------------- OBSERVER ------------------------ */
    event_loop& owner_loop() { return owner_loop_; }
    void        assert_in_loop_thread();
    bool        is_in_loop_thread();

    int  fd() { return pchannel_->fd(); }
    int  events() const { return pchannel_->events(); }
    bool channel_is_reading() const { return pchannel_->is_reading(); }
    bool channel_is_writing() const { return pchannel_->is_writing(); }

protected:
    /* ---------------------- EVENT HANDLER ---------------------- */
    virtual void handle_read(const time_point& when){};
    virtual void handle_write(const time_point& when){};
    virtual void handle_close(const time_point& when){};
    virtual void handle_error(const time_point& when){};

private:
    /* --------------------- CHANNEL MODIFIER -------------------- */

    /// @brief bind the @c channel_ to fd, and set callbacks
    /// @warning this is NOT a thread safe function
    void bind_this_channel_to_fd(int);

    /// @brief clean current @c channel_ and rebind to another fd.
    /// @see bind_this_channel_to_fd;
    void rebind_channel_in_loop(int);

    event_loop&              owner_loop_;
    std::unique_ptr<channel> pchannel_;
};

} // namespace m

#endif // __CHANNELABLE_H__