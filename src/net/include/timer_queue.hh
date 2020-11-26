#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "timer.hh"
#include "pollable.hh"

namespace m
{

class event_loop;

class timer_queue
    : pollable
{
public:
    timer_queue(event_loop& loop);
    ~timer_queue();

    timer_ptr add_timer(
        timer_callback, time_point, time_duration);

protected:
    void handle_read(const time_point&) override;
    
private:
    typedef std::multimap<time_point, timer_ptr> timer_map_t_;
    typedef std::vector<timer_ptr>                 timer_list_t_;

    void reset_timers(const timer_list_t_& expired, time_point now);
    void reset_expieration_from_now(time_point);

    bool          insert(timer_ptr);
    timer_list_t_ pop_expired(time_point now);

    timer_map_t_ timers_;
};

} // namespace m

#endif // __TIMER_QUEUE_H__