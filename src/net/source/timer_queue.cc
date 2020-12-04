#include "timer_queue.hh"

#include <algorithm>
#include <sys/timerfd.h>

#include "event_loop.hh"
#include "logger.hh"
#include "sys_fd.hh"

namespace m
{

void timer_handle::cancel_timer()
{
    loop_.cancel_timer(timer_);
}

} // namespace m

namespace m
{

timer_queue::timer_queue(event_loop& loop)
    : loop_(loop)
    , pollee_(loop_, this, fd::create_timer_fd())
{
    pollee_.enable_reading();
}

timer_queue::~timer_queue()
{
}

timer_handle timer_queue::add_timer(
    timer_callback cb, time_point tp, time_duration intervel)
{
    timer_ptr ptimer = std::make_shared<timer>(cb, tp, intervel);

    // extend lifetime of ptimer untial successfully add it.
    loop_.run_in_loop(
        [&, ptimer]() { add_timer_in_loop(ptimer); });

    return timer_handle(loop_, *ptimer);
}

void timer_queue::cancel_timer(timer& rtimer)
{
    loop_.run_in_loop(
        [&]() { cancel_timer_in_loop(rtimer); });
}

void timer_queue::handle_read(const time_point&)
{
    loop_.assert_in_loop_thread();

    fd::none_reaturn_read(pollee_.fd());

    time_point now      = clock::now();
    timer_list expiered = pop_expired(now);
    for ( auto timer_ptr : expiered )
    {
        timer_ptr->run();
    }

    reset_timers(expiered, now);

    if ( !timers_.empty() )
    {
        time_point next_expiration = timers_.begin()->first;
        reset_expieration_from_now(next_expiration);
    }
}

void timer_queue::add_timer_in_loop(timer_ptr ptimer)
{
    loop_.assert_in_loop_thread();

    bool is_the_first = insert(ptimer);
    if ( is_the_first )
    {
        time_point expiration = ptimer->expiration();
        reset_expieration_from_now(expiration);
    }
}

void timer_queue::cancel_timer_in_loop(timer& rtimer)
{
    loop_.assert_in_loop_thread();

    time_point expired_time = rtimer.expiration();
    auto [it_beg, it_end]   = timers_.equal_range(expired_time);

    auto it = std::find_if(it_beg, it_end,
                           [&](const auto& item) {
                               auto const& [key, value] = item;
                               return value.get() == &rtimer;
                           });
    if ( it != it_end )
    {
        INFO << "erase " << it->second->sequence();
        timers_.erase(it);
    }
}

void timer_queue::reset_timers(const timer_list timers, time_point now)
{
    for ( timer_ptr timer : timers )
    {
        if ( timer->reapeat() )
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
    m::fd::reset_timerfd_time(pollee_.fd(), sec, round_nano_sec);
}

timer_queue::timer_list timer_queue::pop_expired(time_point now)
{
    timer_list expired_timers;

    auto end = timers_.lower_bound(now);
    for ( auto it = timers_.begin(); it != end; ++it )
    {
        expired_timers.push_back(it->second);
    }
    timers_.erase(timers_.begin(), end);

    return expired_timers;
}

bool timer_queue::insert(timer_ptr atimer)
{
    loop_.assert_in_loop_thread();

    bool       first_timer  = false;
    time_point expired_time = atimer->expiration();

    if ( auto it = timers_.begin();
         it == timers_.end() || expired_time < it->first )
    {
        first_timer = true;
    }

    timers_.insert(std::make_pair(expired_time, atimer));
    return first_timer;
}

} // namespace m
