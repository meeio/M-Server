#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "channel.hh"
#include "timer.hh"

namespace m
{

class event_loop;
// class timer;

class timer_queue
{
public:
    timer_queue(event_loop* loop);
    ~timer_queue();

    t_timer_ptr add_timer(t_timer_callback, t_time_point, long intervel);

private:
    typedef std::multimap<t_time_point, t_timer_ptr> t_timer_map_;
    typedef std::vector<t_timer_ptr>                 t_timer_list_;

    void          handel_read();
    t_timer_list_ pop_expired(t_time_point now);
    void          reset(const t_timer_list_& expired, t_time_point now);

    void handle_read();
    // void

    bool insert(t_timer_ptr);

    event_loop* loop_;
    int         timerfd_;
    channel     timerfd_channel_;

    t_timer_map_ timers_;
};

} // namespace m

#endif // __TIMER_QUEUE_H__