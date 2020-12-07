#ifndef __H_EVENT_LOOP__
#define __H_EVENT_LOOP__

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "copytype.hh"
#include "current_thread.hh"
#include "logger.hh"
#include "poller.hh"
#include "poller_waker.hh"
#include "timer.hh"
#include "timer_queue.hh"

namespace m
{

class poll_handle;
class spoller;
class timer_queue;

/* ----------------------------------------------------------- */
/*                       CLASS EVENT_LOOP                      */
/* ----------------------------------------------------------- */

class event_loop
    : noncopyable
{
public:
    typedef std::function<void()> functor;

    event_loop();
    ~event_loop();

    std::string to_string()
    {
        return fmt::format("[TID:{}]", thread_id_);
    }

    /* ------------------------- OBSERVER ------------------------ */

    void assert_in_loop_thread()
    {
        assert(is_in_loop_thread() && "not in thread falst");
    }

    bool is_in_loop_thread() const
    {
        return thread_id_ == current_thread::tid();
    }

    /* --------------------- LOOPING FUNCTION -------------------- */

    void loop();
    void quit();

    /* --------------------- FUNCTION RUNNER --------------------- */

    timer_handle run_at(timer_callback, time_point);
    timer_handle run_after(timer_callback, int);
    timer_handle run_every(timer_callback, int);
    void         cancel_timer(timer&);

    void run_in_loop(functor);
    void queue_in_loop(functor);

    /* --------------------- CHANNEL MANNAGER -------------------- */

    poll_handle& register_polling(int);
    void update_poll_handle(poll_handle&);
    void remove_poll_handle(poll_handle&);

private:
    /* --------------------- FUNCTION RUNNER --------------------- */

    void do_pending_functors();

    /* ---------------------------- V ---------------------------- */

    const int thread_id_;

    int          poll_time_ms_;
    poller::ptr  poller_;
    poller_waker poller_waker_;

    timer_queue timers_;

    bool      looping_;
    bool      quit_;
    bool      calling_pending_functor_;

    std::vector<functor> pending_functors_;
    std::mutex           pending_functors_mutex_;
};

} // namespace m

#endif // !__EVENT_LOOP__