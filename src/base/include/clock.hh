#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <chrono>
#include <string>

#include <cmath>
#include <tuple>

namespace m
{

typedef std::chrono::steady_clock::time_point time_point;
typedef std::chrono::steady_clock::duration   time_duration;

namespace clock
{

std::string time_point_to_str(time_point);

std::tuple<int, long> parse_time_duration(time_duration dur);

time_duration get_duration_ms(int);

inline time_point now() { return std::chrono::steady_clock::now(); }

} // namespace clock

} // namespace m

#endif // __CLOCK_H__