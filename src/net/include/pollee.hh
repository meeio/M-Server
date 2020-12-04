#ifndef __POLL_HANDLE_H__
#define __POLL_HANDLE_H__

#include <memory>

#include "channel.hh"
#include "pollee.hh"
#include "copytype.hh"

namespace m
{


class event_loop;
class pollable;

/* ----------------------------------------------------------- */
/*                      CLASS POLL_HANDLE                      */
/* ----------------------------------------------------------- */

class pollee
    : noncopyable
{
public:
    pollee(event_loop&, pollable*, int);
    ~pollee();

    /* --------------------- CHANNEL MODIFIER -------------------- */

    /// @brief thread safe rebind channel to fd.
    void rebind_channel(int);

    /* ---------------------- EVENT CHANGER ---------------------- */
    void enable_reading() { channel_.enable_reading(); }
    void enable_writing() { channel_.enable_writing(); }
    void disable_writing() { channel_.disable_writing(); }
    void disable_all() { channel_.disable_all(); }

    /* ------------------------- OBSERVER ------------------------ */

    int  fd() { return channel_.fd(); }
    int  events() const { return channel_.events(); }
    bool is_reading() const { return channel_.is_reading(); }
    bool is_writing() const { return channel_.is_writing(); }

private:
    event_loop& loop_;
    pollable*   pollable_;
    channel     channel_;
};

/* ----------------------------------------------------------- */
/*                        CLASS POLLABLE                       */
/* ----------------------------------------------------------- */

class pollable
    : noncopyable
{
protected:
    friend class pollee;

    virtual void handle_read(const time_point& when){};
    virtual void handle_write(const time_point& when){};
    virtual void handle_close(const time_point& when){};
    virtual void handle_error(const time_point& when){};

};

} // namespace m
#endif // __POLL_HANDLE_H__