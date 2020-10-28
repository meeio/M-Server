#include "event_loop.hh"

#include <iostream>
#include <unistd.h>
#include <sys/eventfd.h>

#include "channel.hh"
#include "current_thread.hh"
#include "logger.hh"
#include "poller.hh"
#include "timer_queue.hh"

int cread_fd()
{
    return ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
}

namespace m
{

__thread event_loop* __loop_of_this_thread = nullptr;

event_loop::event_loop()
    : thread_id_(current_thread::tid())
    , looping_(false)
    , poll_time_ms_(1000)
    , timers_(this)
{
    poller_ = std::make_unique<poller>(this);
    if (__loop_of_this_thread != nullptr)
        ERR << "another event_loop object has been cread "
            << "in this thread " << thread_id_;
    else
        __loop_of_this_thread = this;
}

event_loop::~event_loop()
{
}

void event_loop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wake_up_fd_, &one, sizeof one);
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
    quit_ = false;

    TRACE << "event_loop " << this << " star looping.";

    poller::t_channel_vector active_channels;
    while (!quit_)
    {
        active_channels.clear();
        auto time = poller_->poll(poll_time_ms_, active_channels);
        for (channel* ch : active_channels)
        {
            ch->handle_event();
        }
        do_pending_functors();
    }

    poll(NULL, 0, 5 * 1000);

    TRACE << "event_loop " << this << " stop looping.";
    looping_ = false;
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
    poller_->update_channel(ch);
}

t_timer_ptr event_loop::run_at(t_timer_callback cb, t_time_point tp)
{
    return timers_.add_timer(cb, tp, 0);
}

t_timer_ptr event_loop::run_after(t_timer_callback cb, int delay)
{
    t_time_point when = clock::now() + t_time_duration(delay * 1000);
    return run_at(cb, when);
}

t_timer_ptr event_loop::run_every(t_timer_callback cb, int intervel)
{
    t_time_point when = clock::now() + t_time_duration(intervel * 1000);
    return timers_.add_timer(cb, when, intervel);
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
