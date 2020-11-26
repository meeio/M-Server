#include "timer.hh"

namespace m
{

std::atomic_int timer::s_num_created_(0);

timer::timer(timer_callback cb,
             time_point     when,
             time_duration  interval = time_duration(0))
    : callback_(cb)
    , expiration_(when)
    , interval_(interval)
    , reapeat_(interval_.count() > 0)
    , sequence_(++s_num_created_)
{
}

void timer::restart(time_point now)
{
    if (reapeat_)
    {
        expiration_ = now + interval_;
    }
}

} // namespace m
