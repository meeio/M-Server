#include "channel.hh"

#include <poll.h>

#include "event_loop.hh"
#include "logger.hh"

namespace m
{
const int channel::k_none_event  = 0;
const int channel::k_read_evnet  = POLLIN | POLLPRI;
const int channel::k_write_event = POLLOUT;

channel::channel(event_loop* owner_loop, int fd)
    : owner_loop_(owner_loop)
    , fd_(fd)
    , events_(k_none_event)
    , revents_(k_none_event)
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
    owner_loop_->update_channel(this);
}

void channel::handle_event(const time_point_t& poll_time)
{
    handling_event_ = true;

    if (revents_ & POLLNVAL)
    {
        WARN << "Invalid request: fd " << fd_ << " not open.";
    }

    if ((revents_ & (POLLERR | POLLNVAL)))
    {
        if (err_callback_)
            err_callback_();
    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (close_callback_)
            close_callback_();
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
        if (read_callback_)
            read_callback_(poll_time);

    if (revents_ & (POLLOUT))
        if (write_callback_)
            write_callback_();

    handling_event_ = false;
}

void channel::enable_reading()
{
    events_ |= k_read_evnet;
    update();
}

void channel::disable_all()
{
    events_ = k_none_event;
    update();
}

} // namespace m
