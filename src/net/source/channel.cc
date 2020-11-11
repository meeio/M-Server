#include "channel.hh"

#include <poll.h>

#include "event_loop.hh"

namespace m
{

const int channel::k_none_event = 0;
const int channel::k_read_evnet = POLLIN | POLLPRI;
const int channel::k_write_event = POLLOUT;

channel::channel(event_loop* owner_loop, int fd)
    : owner_loop_(owner_loop)
    , fd_(fd)
    , events_(k_none_event)
    , revents_(k_none_event)
    , index_(-1)
{
}

void channel::update()
{
    owner_loop_->update_channel(this);
}

void channel::handle_event()
{
    if (revents_ & (POLLERR | POLLNVAL))
        if (err_callback)
            err_callback();

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
        if (read_callback)
            read_callback();

    if (revents_ & (POLLOUT))
        if (write_callback)
            write_callback();
}



void channel::enable_reading() 
{
    revents_ |= k_read_evnet;
    update();
}


} // namespace m
