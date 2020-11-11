#include "timer.hh"

namespace m
{

std::atomic_int timer::s_num_created_(0);

timer::timer(t_timer_callback cb,
             time_point_t     when,
             time_duration_t  interval = time_duration_t(0))
    : callback_(cb)
    , expiration_(when)
    , interval_(interval)
    , reapeat_(interval_.count() > 0)
    , sequence_(++s_num_created_)
{
}

void timer::restart(time_point_t now)
{
    if (reapeat_)
    {
        expiration_ = now + interval_;
    }
}

} // namespace m
