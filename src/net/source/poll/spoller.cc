#include "poll/spoller.hh"

#include <algorithm>
#include <poll.h>

#include "clock.hh"
#include "event_loop.hh"
#include "logger.hh"
#include "poll_handle.hh"

namespace m
{

spoller::spoller(event_loop& owner)
    : poller(owner)
{
}

spoller::~spoller()
{
}

poller::handle_vector spoller::poll(int timeout_ms)
{
    loop().assert_in_loop_thread();

    int event_num = ::poll(
        &*pollfds_.begin(), pollfds_.size(), timeout_ms);
    
    return find_active_handles(event_num);
}

poll_handle& spoller::register_polling(int fd)
{

    auto emplace_res = handles_.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(fd),
                                        std::forward_as_tuple(loop(), fd));

    assert(emplace_res.second == true);
    assert(handles_.size() == pollfds_.size() + 1);

    poll_handle& handle = emplace_res.first->second;
    handle.set_index(pollfds_.size());

    pollfd apollfd = {
        .fd      = IGNORED_FD,
        .events  = 0,
        .revents = 0};
    pollfds_.push_back(apollfd);

    return handle;
}

poller::handle_vector spoller::find_active_handles(int event_num)
{
    handle_vector ret_handles;
    ret_handles.reserve(event_num);

    for ( const auto pollfd : pollfds_ )
    {
        if ( auto re = pollfd.revents; re > 0 )
        {
            poll_handle& handle = handles_.find(pollfd.fd)->second;
            handle.set_revents(re);
            ret_handles.push_back(handle);
            if ( --event_num == 0 )
                break;
        }
    }

    return ret_handles;
}

void spoller::update_handle(poll_handle& handle)
{
    assert_in_loop_thread();
    assert(handle.index() >= 0);

    int idx = handle.index();

    pollfd& apollfd = pollfds_[idx];
    apollfd.revents = 0;
    apollfd.events  = handle.events();

    if ( handle.is_none_event() )
    {
        apollfd.fd = IGNORED_FD;
    }
    else
    {
        apollfd.fd = handle.fd();
    }
}

void spoller::remove_handle(poll_handle& handle)
{

    assert(&handles_[handle.fd()] == &handle);
    assert(handle.is_none_event());

    // remove channel from pollfds_
    if ( size_t tar_idx = handle.index();
         tar_idx != pollfds_.size() - 1 )
    {
        int tail_idx = pollfds_.size() - 1;
        std::swap(pollfds_[tail_idx], pollfds_[tar_idx]);
    }

    pollfds_.pop_back();
    handles_.erase(handle.fd());
}

} // namespace m
