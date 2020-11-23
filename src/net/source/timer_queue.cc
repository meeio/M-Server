#include "timer_queue.hh"

#include <sys/timerfd.h>

#include "event_loop.hh"
#include "logger.hh"
#include "sys_fd.hh"

namespace m
{

timer_queue::timer_queue(event_loop* loop)
    : channelable(loop, fd::create_timer_fd())
    , timers_()
{
    channel_enable_reading();
}

timer_queue::~timer_queue()
{
}

void timer_queue::handle_read(const time_point_t&)
{
    assert_in_loop_thread();

    fd::read_fd(fd());

    time_point_t  now      = clock::now();
    timer_list_t_ expiered = pop_expired(now);
    for (auto timer_ptr : expiered)
    {
        timer_ptr->run();
    }
    reset_timers(expiered, now);

    if (!timers_.empty())
    {
        time_point_t next_expiration = timers_.begin()->first;
        reset_expieration_from_now(next_expiration);
    }
}

void timer_queue::reset_timers(const timer_list_t_& timers, time_point_t now)
{
    for (timer_ptr_t timer : timers)
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

void timer_queue::reset_expieration_from_now(time_point_t expir)
{
    m::time_duration_t dura_to_expira = expir - m::clock::now();

    auto [sec, nano_sec] = m::clock::parse_time_duration(dura_to_expira);

    nano_sec = nano_sec > 1e6 ? nano_sec : 1e6;
    m::fd::reset_timerfd_time(fd(), sec, nano_sec);
}

std::vector<timer_ptr_t> timer_queue::pop_expired(time_point_t now)
{
    std::vector<timer_ptr_t> expired_timers;
    auto                     end = timers_.lower_bound(now);
    for (auto it = timers_.begin(); it != end; ++it)
    {
        expired_timers.push_back(it->second);
    }
    timers_.erase(timers_.begin(), end);
    return expired_timers;
}

timer_ptr_t timer_queue::add_timer(
    t_timer_callback cb, time_point_t tp, time_duration_t intervel)
{
    timer_ptr_t ptimer = std::make_shared<timer>(cb, tp, intervel);

    loop()->run_in_loop([&]() {
        bool is_the_first = insert(ptimer);
        if (is_the_first)
        {
            time_point_t expiration = ptimer->expiration();
            reset_expieration_from_now(expiration);
        }
    });

    return ptimer;
}

bool timer_queue::insert(timer_ptr_t atimer)
{
    assert_in_loop_thread();

    bool         first_timer  = false;
    time_point_t expired_time = atimer->expiration();

    if (auto it = timers_.begin();
        it == timers_.end() || expired_time < it->first)
    {
        first_timer = true;
    }

    timers_.insert(std::make_pair(expired_time, atimer));
    return first_timer;
}

} // namespace m
