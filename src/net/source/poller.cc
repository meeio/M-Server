#include "poller.hh"

#include "event_loop.hh"

#include "poll/spoller.hh"

namespace m
{

poller::poller(event_loop& loop)
    : owner_loop_(loop)
{
}

poller::~poller()
{
}


poller::ptr poller::create_poller(event_loop& loop) 
{
    return std::make_unique<spoller>(loop);
}


void poller::assert_in_loop_thread()
{
    loop().assert_in_loop_thread();
}

} // namespace m

//