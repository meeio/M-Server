#include "timer.hh"

namespace m
{

std::atomic_int timer::s_num_created_(0);

timer::timer(t_timer_callback cb, t_time_point when, double millisecond_interval)
    : callback_(cb)
    , expiration_(when)
    , interval_(t_time_duration(static_cast<long>(millisecond_interval * 1e6)))
    , reapeat_(millisecond_interval > 0)
    , sequence_(++s_num_created_)
{
}

void timer::restart(t_time_point now)
{
    if (reapeat_)
    {
        expiration_ = now + interval_;
    }
}

} // namespace m
