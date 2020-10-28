#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <chrono>
#include <string>

#include <cmath>
#include <tuple>

namespace m
{

typedef std::chrono::steady_clock::time_point t_time_point;
typedef std::chrono::steady_clock::duration   t_time_duration;

namespace clock
{

std::string time_point_to_str(t_time_point);

std::tuple<int, long> parse_time_duration(t_time_duration dur);

inline t_time_point now() { return std::chrono::steady_clock::now(); }

} // namespace clock

} // namespace m

#endif // __CLOCK_H__