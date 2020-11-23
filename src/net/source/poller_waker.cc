#include "poller_waker.hh"
#include "sys_fd.hh"
#include "poller.hh"
namespace m
{


poller_waker::poller_waker(poller& p)
    : channelable(p.loop(), fd::create_fd())
{
    channel_enable_reading();
}


poller_waker::~poller_waker() 
{

}

/**
 * * this is a function to wakeup thread when blocking
 * * in 'poll()' method.
 * * this happents when (1). aleardy blocked in poll() 
 * * or (2). will block in next loop. 
 */
void poller_waker::wakeup_loop()
{
    assert( events() & channel::k_read_evnet );
    uint64_t one = 1;
    ssize_t  n   = ::write(fd(), &one, sizeof one);
    if (n != sizeof one)
    {
        ERR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}


void poller_waker::handle_read(const time_point_t&) 
{
    fd::read_fd(fd());
}

    
} // namespace m
