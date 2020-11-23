#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "channel.hh"
#include "timer.hh"
#include "channelable.hh"

namespace m
{

class event_loop;
// class timer;

class timer_queue
    : channelable
{
public:
    timer_queue(event_loop* loop);
    ~timer_queue();

    timer_ptr_t add_timer(
        t_timer_callback, time_point_t, time_duration_t);

private:
    typedef std::multimap<time_point_t, timer_ptr_t> timer_map_t_;
    typedef std::vector<timer_ptr_t>                 timer_list_t_;

    void handle_read(const time_point_t&);
    void reset_timers(const timer_list_t_& expired, time_point_t now);
    void reset_expieration_from_now(time_point_t);

    bool          insert(timer_ptr_t);
    timer_list_t_ pop_expired(time_point_t now);

    // event_loop* loop_;
    // int         timerfd_;
    // channel     timerfd_channel_;

    timer_map_t_ timers_;
};

} // namespace m

#endif // __TIMER_QUEUE_H__