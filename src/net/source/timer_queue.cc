#include "timer_queue.hh"

#include <sys/timerfd.h>

#include "event_loop.hh"
#include "logger.hh"
#include "sys_fd.hh"

namespace m
{

timer_queue::timer_queue(event_loop& loop)
    : pollable(loop, fd::create_timer_fd())
{
    channel_enable_reading();
}

timer_queue::~timer_queue()
{
}

void timer_queue::handle_read(const time_point&)
{
    assert_in_loop_thread();

    fd::none_reaturn_read(fd());

    time_point  now      = clock::now();
    timer_list_t_ expiered = pop_expired(now);
    for (auto timer_ptr : expiered)
    {
        timer_ptr->run();
    }
    reset_timers(expiered, now);

    if (!timers_.empty())
    {
        time_point next_expiration = timers_.begin()->first;
        reset_expieration_from_now(next_expiration);
    }
}

void timer_queue::reset_timers(const timer_list_t_& timers, time_point now)
{
    for (timer_ptr timer : timers)
    {
        if (timer->reapeat())
        {
            timer->restart(now);
            insert(timer);
        }
        else
        {
            // delete timer ?
        }
    }
}

void timer_queue::reset_expieration_from_now(time_point expir)
{
    m::time_duration dura_to_expira = expir - m::clock::now();

    auto [sec, nano_sec] = m::clock::parse_time_duration(dura_to_expira);

    int round_nano_sec = (sec == 0 && nano_sec < 1e6) ? 1e6 : nano_sec;
    m::fd::reset_timerfd_time(fd(), sec, round_nano_sec);
}

std::vector<timer_ptr> timer_queue::pop_expired(time_point now)
{
    std::vector<timer_ptr> expired_timers;

    auto end = timers_.lower_bound(now);
    for (auto it = timers_.begin(); it != end; ++it)
    {
        expired_timers.push_back(it->second);
    }
    timers_.erase(timers_.begin(), end);

    return expired_timers;
}

timer_ptr timer_queue::add_timer(
    timer_callback cb, time_point tp, time_duration intervel)
{
    timer_ptr ptimer = std::make_shared<timer>(cb, tp, intervel);

    owner_loop().run_in_loop([&]() {
        bool is_the_first = insert(ptimer);
        if (is_the_first)
        {
            time_point expiration = ptimer->expiration();
            reset_expieration_from_now(expiration);
        }
    });

    return ptimer;
}

bool timer_queue::insert(timer_ptr atimer)
{
    assert_in_loop_thread();

    bool         first_timer  = false;
    time_point expired_time = atimer->expiration();

    if (auto it = timers_.begin();
        it == timers_.end() || expired_time < it->first)
    {
        first_timer = true;
    }

    timers_.insert(std::make_pair(expired_time, atimer));
    return first_timer;
}

} // namespace m
