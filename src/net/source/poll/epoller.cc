#include "poll/epoller.hh"

#include <sys/epoll.h>

namespace m
{

epoller::epoller(event_loop& loop)
    : poller(loop)
    , epoll_fd_(epoll_create1(EPOLL_CLOEXEC))
    , retrive_events_(1)
{
}

poller::handle_vector epoller::poll(int timeout_ms)
{
    // TODO: cheack epoll wait

    retrive_events_.clear();

    int event_num = epoll_wait(epoll_fd_,
                               retrive_events_.data(),
                               retrive_events_.size(),
                               timeout_ms);

    if ( event_num ==  retrive_events_.size())
    {
        retrive_events_.resize(retrive_events_.size() * 2);
    }

    handle_vector res;
    for ( auto& event : retrive_events_ )
    {
        poll_handle* phandle = static_cast<poll_handle*>(event.data.ptr);
        res.push_back(*phandle);
    }

    return res;
}



} // namespace m
