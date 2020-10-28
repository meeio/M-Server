#include "clock.hh"

namespace m::clock
{
    
std::tuple<int, long> parse_time_duration(t_time_duration dur)
{
    long nano_count = dur.count();
    int  sec = nano_count / 1e9;
    long nano = nano_count - (sec * 1e9);

    return {sec, nano};
}

} // namespace m::clock

