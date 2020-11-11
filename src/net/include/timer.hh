#ifndef __TIMER_H__
#define __TIMER_H__

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

#include "clock.hh"

namespace m
{

typedef std::function<void()>        t_timer_callback;
typedef std::shared_ptr<class timer> timer_ptr_t;

namespace time
{
time_point_t now();
} // namespace time

class timer
{
public:
    timer(t_timer_callback, time_point_t, time_duration_t);

    void run() { callback_(); }
    void restart(time_point_t);

    time_point_t expiration() { return expiration_; }
    const int    sequence() { return sequence_; }
    bool         reapeat() { return reapeat_; }

private:
    t_timer_callback callback_;
    time_duration_t  interval_;
    time_point_t     expiration_;
    bool             reapeat_;
    const int        sequence_;

    static std::atomic_int s_num_created_;
};

} // namespace m

#endif // __TIMER_H__