#include "clock.hh"

namespace m::clock
{

std::tuple<int, long> parse_time_duration(time_duration_t dur)
{
    long nano_count = dur.count();
    int  sec        = nano_count / 1e9;
    long nano       = nano_count - (sec * 1e9);

    return {sec, nano};
}

time_duration_t get_time_duration(int milliseconds)
{
    long in_nano = milliseconds * 1e6;
    return time_duration_t(in_nano);
}

std::string time_point_to_str(time_point_t tp)
{

    std::time_t t = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now() -
        (tp - now()));
    return std::string(std::ctime(&t));
}

} // namespace m::clock
