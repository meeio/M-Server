#include "poller_waker.hh"

#include "pollee.hh"
#include "poller.hh"
#include "sys_fd.hh"

namespace m
{

poller_waker::poller_waker(poller& p)
    : pollee_(p.loop(), this, fd::create_fd())
{
    pollee_.enable_reading();
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
    assert(pollee_.is_reading());
    uint64_t one = 1;
    ssize_t  n   = ::write(pollee_.fd(), &one, sizeof one);
    if ( n != sizeof one )
    {
        ERR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void poller_waker::handle_read(const time_point&)
{
    fd::none_reaturn_read(pollee_.fd());
}

} // namespace m
