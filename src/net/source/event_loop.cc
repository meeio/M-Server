#include "event_loop.hh"

#include "channel.hh"
#include "current_thread.hh"
#include "logger.hh"
#include "poller.hh"
#include "sys_fd.hh"
#include "timer_queue.hh"
#include "poller_waker.hh"

namespace m
{

thread_local event_loop* __loop_of_this_thread = nullptr;

event_loop::event_loop()
    : thread_id_(current_thread::tid())
    , looping_(false)
    , poll_time_ms_(1000)
    , timers_(*this)
    , poller_(*this)
    , poller_waker_(poller_)
{    
    if (__loop_of_this_thread != nullptr)
        ERR << "another event_loop object has been cread "
            << "in this thread " << thread_id_;
    else
        __loop_of_this_thread = this;
}

event_loop::~event_loop()
{
}

void event_loop::loop()
{
    assert(!looping_);
    assert_in_loop_thread();

    looping_ = true;
    quit_    = false;

    TRACE << "event_loop " << to_string() << " star looping.";

    while (!quit_)
    {
        poller::channel_vector_t active_channels = poller_.poll(poll_time_ms_);

        time_point poll_time = clock::now();

        for (channel* ch : active_channels)
        {
            ch->handle_event(poll_time);
        }
        do_pending_functors();
    }

    looping_ = false;

    TRACE << "event_loop" << to_string() << " stop looping.";
}

void event_loop::quit()
{
    quit_ = true;
    if (!is_in_loop_thread())
    {
        poller_waker_.wakeup_loop();
    }
}

void event_loop::update_channel(channel* ch)
{
    assert(&ch->owner_loop() == this);
    assert_in_loop_thread();
    poller_.update_channel(ch);
}

void event_loop::remove_channel(channel* ch)
{
    assert(&ch->owner_loop() == this);
    assert_in_loop_thread();
    poller_.remove_channel(ch);
}

timer_handle event_loop::run_at(timer_callback cb, time_point tp)
{
    time_duration dur = clock::get_duration_from_ms(0);
    return timers_.add_timer(cb, tp, dur);
}

timer_handle event_loop::run_after(timer_callback cb, int delay_ms)
{
    time_point    when       = clock::now();
    time_duration delay_dura = clock::get_duration_from_ms(delay_ms);
    return run_at(cb, when + delay_dura);
}

timer_handle event_loop::run_every(timer_callback cb, int intervel_ms)
{
    time_point    when       = clock::now();
    time_duration inter_dura = clock::get_duration_from_ms(intervel_ms);
    return timers_.add_timer(cb, when + inter_dura, inter_dura);
}

void event_loop::cancel_timer(timer& rtimer) 
{
    timers_.cancel_timer(rtimer);
}

void event_loop::run_in_loop(functor fc)
{
    if (is_in_loop_thread())
    {
        fc();
    }
    else
    {
        queue_in_loop(fc);
    }
}

void event_loop::queue_in_loop(functor fc)
{
    {
        std::lock_guard<std::mutex> lock(pending_functors_mutex_);
        pending_functors_.push_back(fc);
    }

    if (!is_in_loop_thread() || calling_pending_functor_)
    {
        poller_waker_.wakeup_loop();
    }
}

void event_loop::do_pending_functors()
{
    std::vector<functor> functors;
    calling_pending_functor_ = true;

    {
        std::lock_guard<std::mutex> lock(pending_functors_mutex_);
        functors.swap(pending_functors_);
    }

    for (auto fc : functors)
    {
        fc();
    }
    calling_pending_functor_ = false;
}

} // namespace m
