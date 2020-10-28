#ifndef __H_EVENT_LOOP__
#define __H_EVENT_LOOP__

#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "current_thread.hh"
#include "timer.hh"
#include "timer_queue.hh"
// #include "poller.hh"

namespace m
{

class channel;
class poller;
class timer_queue;

class event_loop
{
public:
    typedef std::function<void()> functor;

    event_loop();
    ~event_loop();

    void loop();
    void quit();

    t_timer_ptr run_at(t_timer_callback, t_time_point);
    t_timer_ptr run_after(t_timer_callback, int);
    t_timer_ptr run_every(t_timer_callback, int);

    void run_in_loop(functor);
    void queue_in_loop(functor);
    void wakeup();

    void update_channel(channel*);

    void assert_in_loop_thread()
    {
        if (!is_in_loop_thread())
            assert(false);
    }

    bool is_in_loop_thread() const
    {
        return thread_id_ == current_thread::tid();
    }

private:
    void handle_read();
    void do_pending_functors();

    bool      looping_;
    bool      quit_;
    bool      calling_pending_functor_;
    const int thread_id_;

    std::unique_ptr<poller> poller_;
    int                     poll_time_ms_;

    timer_queue timers_;

    int                      wake_up_fd_;
    std::unique_ptr<channel> wake_up_channel_;

    std::mutex           pending_functors_mutex_;
    std::vector<functor> pending_functors_;
};

} // namespace m

#endif // !__EVENT_LOOP__