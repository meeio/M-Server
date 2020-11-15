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

poller::channel_vector_t&  poller::poll(int timeout_ms)
{
    int event_num = ::poll(
        &*pollfds_.begin(), pollfds_.size(), timeout_ms);
    // time_point_t now = clock::now();
    return find_active_channel(event_num);
}

poller::channel_vector_t&  poller::find_active_channel(int event_num)
{
    static channel_vector_t ret_channels;
    ret_channels.clear();

    for (const auto pollfd : pollfds_)
    {
        if (auto re = pollfd.revents ; re > 0)
        {
            const auto ch_it = channels_.find(pollfd.fd);
            channel*   ch    = ch_it->second;
            ch->set_revents(re);

            ret_channels.push_back(ch);
            if (--event_num == 0)
                break;
        }
    }

    return ret_channels;
}

void poller::update_channel(channel* ch)
{
    assert_in_loop_thread();

    if (ch->index() < 0)
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

        if (ch->is_none_event())
            apollfd.fd = -1;
    }
}

void poller::assert_in_loop_thread()
{
    owner_loop_->assert_in_loop_thread();
}

} // namespace m
