#include "poller.hh"

#include "event_loop.hh"
#include "logger.hh"

namespace m
{

poller::poller(event_loop* owner)
    : owner_loop_(owner)
{
}

poller::~poller()
{
}

time_point_t poller::poll(int timeout_ms, channel_vector_t& vector)
{
    int event_num = ::poll(
        &*pollfds_.begin(), pollfds_.size(), timeout_ms);
    time_point_t now = clock::now();
    find_active_channel(event_num, vector);

    return now;
}


void poller::find_active_channel(int event_num, channel_vector_t& vector)
{
    for (const auto pollfd : pollfds_)
    {
        if (pollfd.revents > 0)
        {
            const auto ch_it = channels_.find(pollfd.fd);
            channel*   ch = ch_it->second;
            ch->set_revents(pollfd.revents);
            vector.push_back(ch);
            if (--event_num == 0)
                break;
        }
    }
}

void poller::update_channel(channel* ch)
{
    assert_int_loop_thread();

    if (ch->index() < 0)
    {
        pollfd apollfd = {
            .fd = ch->fd(),
            .events = static_cast<short>(ch->events()),
            .revents = 0};

        pollfds_.push_back(apollfd);
        int idx = pollfds_.size() - 1;
        ch->set_index(idx);
        channels_[ch->fd()] = ch;
    }
    else
    {
        int     idx = ch->index();
        pollfd& apollfd = pollfds_[idx];
        apollfd.revents = 0;
        apollfd.events = ch->events();
        
        if (ch->is_none_event())
            apollfd.fd = -1;
    }
}

void poller::assert_int_loop_thread()
{
    owner_loop_ -> assert_in_loop_thread();
}

} // namespace m
