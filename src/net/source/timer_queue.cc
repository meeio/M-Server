#include "timer_queue.hh"

#include <sys/timerfd.h>

#include "event_loop.hh"
#include "logger.hh"

#include "sys_fd.hh"

void reset_timerfd_from_to_expieration(int timerfd, m::t_time_point expir)
{
    m::t_time_duration dura_to_expira = expir - m::clock::now();
    auto [sec, nano_sec] = m::clock::parse_time_duration(dura_to_expira);
    m::timerfd::reset_timerfd_time(timerfd, sec, nano_sec);
}

namespace m
{

timer_queue::timer_queue(event_loop* loop)
    : loop_(loop)
    , timerfd_(timerfd::create_timer_fd())
    , timerfd_channel_(loop, timerfd_)
    , timers_()
{
    timerfd_channel_.set_read_callback([&]() { handle_read(); });
    timerfd_channel_.enable_reading();
}

timer_queue::~timer_queue()
{
}

std::vector<t_timer_ptr> timer_queue::pop_expired(t_time_point now)
{
    std::vector<t_timer_ptr> expired_timers;
    auto                     end = timers_.lower_bound(now);
    for (auto it = timers_.begin(); it != end; ++it)
    {
        expired_timers.push_back(it->second);
    }
    timers_.erase(timers_.begin(), end);
    return expired_timers;
}

t_timer_ptr timer_queue::add_timer(
    t_timer_callback cb, t_time_point tp, long intervel)
{
    auto ptimer = std::make_shared<timer>(cb, tp, intervel);
    bool is_the_first = insert(ptimer);

    if (is_the_first)
    {
        t_time_point expiration = ptimer->expiration();
        reset_timerfd_from_to_expieration(timerfd_, expiration);
    }

    return ptimer;
}

void timer_queue::handle_read()
{
    loop_->assert_in_loop_thread();
    t_time_point now = clock::now();

    auto expiered = pop_expired(now);
    for (auto timer_ptr : expiered)
    {
        timer_ptr->run();
    }
    reset(expiered, now);
}

void timer_queue::reset(const t_timer_list_& timers, t_time_point now)
{
    bool update_first_timer = false;
    for (t_timer_ptr timer : timers)
    {
        if (timer->reapeat())
        {
            timer->restart(now);
            update_first_timer = update_first_timer || insert(timer);
        }
        else
        {
            // delete timer ?
        }
    }

    if (update_first_timer)
    {
        t_timer_ptr  first_timer = timers_.begin()->second;
        t_time_point next_expira = first_timer->expiration();
        reset_timerfd_from_to_expieration(timerfd_, next_expira);
    }
}

bool timer_queue::insert(t_timer_ptr atimer)
{
    bool         first_timer = false;
    t_time_point expired_time = atimer->expiration();

    if (auto it = timers_.begin();
        it != timers_.end() || expired_time < it->first)
    {
        first_timer = true;
    }
    timers_.insert(std::make_pair(expired_time, atimer));
    return first_timer;
}

} // namespace m
