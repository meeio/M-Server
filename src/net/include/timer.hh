#ifndef __TIMER_H__
#define __TIMER_H__

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

#include "clock.hh"

namespace m
{

typedef std::function<void()>                 t_timer_callback;
typedef std::shared_ptr<class timer>          t_timer_ptr;

namespace time
{
t_time_point now();
} // namespace time

class timer
{
public:
    timer(t_timer_callback, t_time_point, double interval);

    void run() { callback_(); }
    void restart(t_time_point);

    t_time_point expiration() { return expiration_; }
    const int    sequence() { return sequence_; }
    bool         reapeat() { return reapeat_; }

private:
    t_timer_callback  callback_;
    t_time_duration interval_;
    t_time_point      expiration_;
    bool              reapeat_;
    const int         sequence_;

    static std::atomic_int s_num_created_;
};

} // namespace m

#endif // __TIMER_H__