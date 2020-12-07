#ifndef __TIMER_H__
#define __TIMER_H__

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

#include "clock.hh"

namespace m
{

typedef std::function<void()> timer_callback;
// typedef std::shared_ptr<class timer> timer_ptr;

namespace time
{
time_point now();
} // namespace time

class timer
{
public:
    timer(timer_callback, time_point, time_duration);

    void run() { callback_(); }
    void restart(time_point);

    time_point expiration() { return expiration_; }
    int        sequence() { return sequence_; }
    bool       reapeat() { return reapeat_; }

private:
    timer_callback callback_;
    time_duration  interval_;
    time_point     expiration_;
    bool           reapeat_;
    const int      sequence_;

    static std::atomic_int s_num_created_;
};

} // namespace m

#endif // __TIMER_H__