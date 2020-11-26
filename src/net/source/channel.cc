#include "channel.hh"

#include <poll.h>

#include "event_loop.hh"
#include "logger.hh"

namespace m
{
const int channel::EVENT_NONE  = 0;
const int channel::EVENT_READ  = POLLIN | POLLPRI;
const int channel::EVENT_WRITE = POLLOUT;

channel::channel(event_loop& owner_loop, int fd)
    : owner_loop_(owner_loop)
    , fd_(fd)
    , events_(EVENT_NONE)
    , revents_(EVENT_NONE)
    , index_(-1)
    , handling_event_(false)
{
}

channel::~channel()
{
    assert(handling_event_ == false);
    TRACE << "channel::~channel: the channel for "
          << fd_ << " has been delete.";
}

void channel::update()
{
    owner_loop_.update_channel(this);
}

void channel::handle_event(const time_point& poll_time)
{
    handling_event_ = true;

    if (revents_ & POLLNVAL)
    {
        WARN << "Invalid request: fd " << fd_ << " not open.";
    }

    if ((revents_ & (POLLERR | POLLNVAL)))
    {
        if (err_callback_)
            err_callback_(poll_time);
    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (close_callback_)
            close_callback_(poll_time);
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
        if (read_callback_)
            read_callback_(poll_time);

    if (revents_ & (POLLOUT))
        if (write_callback_)
            write_callback_(poll_time);

    handling_event_ = false;
}

void channel::enable_reading()
{
    events_ |= EVENT_READ;
    update();
}


void channel::enable_writing() 
{
    events_ |= EVENT_WRITE;
    update();
}


void channel::disable_writing() 
{
    events_ &= ~EVENT_WRITE;
    update();
}


void channel::disable_all()
{
    events_ = EVENT_NONE;
    update();
}

} // namespace m
