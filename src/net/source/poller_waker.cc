#include "poller_waker.hh"

#include "event_loop.hh"
#include "poll_handle.hh"
#include "sys_fd.hh"

namespace m
{

poller_waker::poller_waker(poller& p)
    : phandle_(p.loop().register_polling(fd::create_fd())) 
{
    phandle_.bind_handler(*this);
    phandle_.enable_reading();
}

poller_waker::~poller_waker()
{
    
}

void poller_waker::wakeup_loop()
{
    assert(phandle_.is_reading());
    uint64_t one = 1;
    ssize_t  n   = ::write(phandle_.fd(), &one, sizeof one);
    if ( n != sizeof one )
    {
        ERR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void poller_waker::handle_read(const time_point&)
{
    fd::none_reaturn_read(phandle_.fd());
}

} // namespace m
