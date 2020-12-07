#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "poll_handler.hh"
#include "timer.hh"

namespace m
{

class poll_handle;
class event_loop;

/* ----------------------------------------------------------- */
/*                      CLASS TIMER_HANDLE                     */
/* ----------------------------------------------------------- */

class timer_handle
{
public:
    timer_handle(event_loop& loop, timer& ptimer)
        : loop_(loop)
        , timer_(ptimer)
    {
    }
    timer& get_timer() { return timer_; }
    void   cancel_timer();

private:
    event_loop& loop_;
    timer&      timer_;
};

/* ----------------------------------------------------------- */
/*                      CLASS TIMER_QUEUE                      */
/* ----------------------------------------------------------- */

class timer_queue
    : poll_handler
{
public:
    timer_queue(event_loop& loop);
    ~timer_queue();

    /// @brief thread-safe function to add timer
    timer_handle add_timer(timer_callback, time_point, time_duration);

    /// @brief thread-safe function to remove timer
    void cancel_timer(timer&);

protected:
    /// @details excutes timers that have expired and then @c restart
    /// timers if it needed, and @c reset_expieration_from_now for
    /// the next read event.
    virtual void handle_read(const time_point&) override;

private:
    /* ------------------------ INNERTYPES ----------------------- */
    typedef std::shared_ptr<class timer>         timer_ptr;
    typedef std::multimap<time_point, timer_ptr> timer_map;
    typedef std::vector<timer_ptr>               timer_list;

    /* --------------------- TIMERS MODIFIERS -------------------- */

    /// @brief add timer in loop is not thread safe but in loop.
    void add_timer_in_loop(timer_ptr);
    void cancel_timer_in_loop(timer&);

    /// @brief reset timers to timeporint if timer need @c restart.
    void reset_timers(const timer_list expired, time_point now);

    /// @brief insert the timer and return ture if this timer is the first
    bool insert(timer_ptr);

    /// @brief return the expired timers by gien time_point
    timer_list pop_expired(time_point now);

    /* -------------------- CHANNEL MODIFIERS -------------------- */

    /// @brief reset this channel's read event to trigge in time.
    void reset_expieration_from_now(time_point);

    /* ------------------------ ATTRIBUTE ------------------------ */

    event_loop&  loop_;
    poll_handle& poll_hd_;
    timer_map    timers_;
};

} // namespace m

#endif // __TIMER_QUEUE_H__