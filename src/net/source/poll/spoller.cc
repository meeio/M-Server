#include "poll/spoller.hh"

#include <algorithm>

#include "channel.hh"
#include "clock.hh"
#include "event_loop.hh"
#include "logger.hh"

namespace m
{

spoller::spoller(event_loop& owner)
    : poller(owner)
{
}

spoller::~spoller()
{
}

poller::channel_vector spoller::poll(int timeout_ms)
{
    int event_num = ::poll(
        &*pollfds_.begin(), pollfds_.size(), timeout_ms);
    // time_point_t now = clock::now();
    return find_active_channel(event_num);
}

poller::channel_vector spoller::find_active_channel(int event_num)
{
    channel_vector ret_channels;

    for ( const auto pollfd : pollfds_ )
    {
        if ( auto re = pollfd.revents; re > 0 )
        {
            const auto ch_it = channels_.find(pollfd.fd);
            channel*   ch    = ch_it->second;
            ch->set_revents(re);

            ret_channels.push_back(ch);
            if ( --event_num == 0 )
                break;
        }
    }

    return ret_channels;
}

void spoller::update_channel(channel* ch)
{
    assert_in_loop_thread();

    if ( ch->index() < 0 )
    {
        pollfd apollfd = {
            .fd      = ch->fd(),
            .events  = static_cast<short>(ch->events()),
            .revents = 0};

        pollfds_.push_back(apollfd);
        ch->set_index(pollfds_.size() - 1);
        channels_[ch->fd()] = ch;
    }
    else
    {
        int idx = ch->index();

        pollfd& apollfd = pollfds_[idx];
        apollfd.revents = 0;
        apollfd.events  = ch->events();

        if ( ch->is_none_event() )
            apollfd.fd = -1;
    }
}

void spoller::remove_channel(channel* ch)
{

    assert(channels_.find(ch->fd()) != channels_.end());
    assert(channels_[ch->fd()] == ch);
    assert(ch->is_none_event());

    // remove channel from pollfds_
    if ( int t_idx = ch->index(); t_idx == pollfds_.size() - 1 )
    {
        pollfds_.pop_back();
    }
    else
    {
        int tail_channel_fd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin() + t_idx, pollfds_.end() - 1);
        channels_[tail_channel_fd]->set_index(t_idx);
        pollfds_.pop_back();
    }

    // remove channel from fd to channel* map
    size_t n = channels_.erase(ch->fd());
    assert(n == 1);
}

} // namespace m
