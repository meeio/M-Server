#include "event_loop.hh"

#include "channel.hh"
#include "current_thread.hh"
#include "logger.hh"
#include "poller.hh"
#include "sys_fd.hh"
#include "timer_queue.hh"

namespace m
{

__thread event_loop* __loop_of_this_thread = nullptr;

event_loop::event_loop()
    : thread_id_(current_thread::tid())
    , looping_(false)
    , poll_time_ms_(1000)
    , timers_(this)
    , poller_(this)
    , wake_up_fd_(fd::cread_fd())
    , wake_up_channel_(this, wake_up_fd_)
{
    if (__loop_of_this_thread != nullptr)
        ERR << "another event_loop object has been cread "
            << "in this thread " << thread_id_;
    else
        __loop_of_this_thread = this;

    wake_up_channel_.set_read_callback([&] {
        fd::read_fd(wake_up_fd_);
    });
    wake_up_channel_.enable_reading();
}

event_loop::~event_loop()
{
}

/**
 * * this is a function to wakeup thread when blocking
 * * in 'poll()' method.
 * * this happents when (1). aleardy blocked in poll() 
 * * or (2). will block in next loop. 
 */
void event_loop::wakeup()
{
    uint64_t one = 1;
    ssize_t  n   = ::write(wake_up_fd_, &one, sizeof one);
    if (n != sizeof one)
    {
        ERR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void event_loop::loop()
{
    assert(!looping_);
    assert_in_loop_thread();

    looping_ = true;
    quit_    = false;

    TRACE << *this << " star looping.";

    while (!quit_)
    {
        poller::channel_vector_t active_channels = poller_.poll(poll_time_ms_);

        for (channel* ch : active_channels)
        {
            ch->handle_event();
        }
        do_pending_functors();
    }

    looping_ = false;
    
    TRACE << *this << " stop looping.";
}

void event_loop::quit()
{
    quit_ = true;
    if (!is_in_loop_thread())
    {
        wakeup();
    }
}

void event_loop::update_channel(channel* ch)
{
    assert(ch->owner_loop() == this);
    assert_in_loop_thread();
    poller_.update_channel(ch);
}


void event_loop::remove_channel(channel* ch) 
{
    assert(ch->owner_loop() == this);
    assert_in_loop_thread();
    poller_.remove_channel(ch);
}


timer_ptr_t event_loop::run_at(t_timer_callback cb, time_point_t tp)
{
    time_duration_t dur = clock::get_time_duration(0);
    return timers_.add_timer(cb, tp, dur);
}

timer_ptr_t event_loop::run_after(t_timer_callback cb, int delay)
{
    time_point_t    when       = clock::now();
    time_duration_t delay_dura = clock::get_time_duration(delay);
    return run_at(cb, when + delay_dura);
}

timer_ptr_t event_loop::run_every(t_timer_callback cb, int intervel)
{
    time_point_t    when       = clock::now();
    time_duration_t inter_dura = clock::get_time_duration(intervel);
    return timers_.add_timer(cb, when + inter_dura, inter_dura);
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
        wakeup();
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
