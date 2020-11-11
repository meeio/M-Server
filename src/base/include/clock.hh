#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <chrono>
#include <string>

#include <cmath>
#include <tuple>

namespace m
{

typedef std::chrono::steady_clock::time_point time_point_t;
typedef std::chrono::steady_clock::duration   time_duration_t;

namespace clock
{

std::string time_point_to_str(time_point_t);

std::tuple<int, long> parse_time_duration(time_duration_t dur);

time_duration_t get_time_duration(int);

inline time_point_t now() { return std::chrono::steady_clock::now(); }

} // namespace clock

} // namespace m

#endif // __CLOCK_H__