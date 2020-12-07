#include "poll_handle.hh"

#include <poll.h>

#include "event_loop.hh"
#include "logger.hh"
#include "poll_handler.hh"

namespace m
{
const int poll_handle::EVENT_NONE  = 0;
const int poll_handle::EVENT_READ  = POLLIN | POLLPRI;
const int poll_handle::EVENT_WRITE = POLLOUT;

poll_handle::poll_handle(event_loop& owner_loop, int fd)
    : owner_loop_(owner_loop)
    , fd_(fd)
    , index_(-1)
    , events_(EVENT_NONE)
    , revents_(EVENT_NONE)
    , handling_event_(false)
{
    assert(fd > 0);
}

poll_handle::~poll_handle()
{
    assert(handling_event_ == false);
    TRACE << "channel::~channel: the channel for "
          << fd_ << " has been delete.";
}

void poll_handle::update()
{
    owner_loop_.update_poll_handle(*this);
}

void poll_handle::bind_handler(poll_handler& phandle)
{
    set_read_callback(
        [&](auto&... args) { phandle.handle_read(args...); });

    set_write_callback(
        [&](auto&... args) { phandle.handle_write(args...); });

    set_close_callback(
        [&](auto&... args) { phandle.handle_close(args...); });

    set_err_callback(
        [&](auto&... args) { phandle.handle_error(args...); });
}

void poll_handle::handle_event(const time_point& poll_time)
{
    handling_event_ = true;

    if ( revents_ & POLLNVAL )
    {
        WARN << "Invalid request: fd " << fd_ << " not open.";
    }

    if ( (revents_ & (POLLERR | POLLNVAL)) )
    {
        if ( err_callback_ )
            err_callback_(poll_time);
    }

    if ( (revents_ & POLLHUP) && !(revents_ & POLLIN) )
    {
        if ( close_callback_ )
            close_callback_(poll_time);
    }

    if ( revents_ & (POLLIN | POLLPRI | POLLRDHUP) )
        if ( read_callback_ )
            read_callback_(poll_time);

    if ( revents_ & (POLLOUT) )
        if ( write_callback_ )
            write_callback_(poll_time);

    handling_event_ = false;
}

void poll_handle::enable_reading()
{
    events_ |= EVENT_READ;
    update();
}

void poll_handle::enable_writing()
{
    events_ |= EVENT_WRITE;
    update();
}

void poll_handle::disable_writing()
{
    events_ &= ~EVENT_WRITE;
    update();
}

void poll_handle::disable_all()
{
    events_ = EVENT_NONE;
    update();
}

} // namespace m
