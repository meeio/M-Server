#include "event_loop.hh"

#include <poll.h>

#include "logger.hh"

namespace m
{

__thread event_loop* __loop_of_this_thread = nullptr;
    
event_loop::event_loop()
    : thread_id_(current_thread::tid())
    , looping_(false)
{
    if (__loop_of_this_thread != nullptr)
        ERR << "another event_loop object has been cread "
            << "in this thread " << thread_id_;
    else
        __loop_of_this_thread = this;
}

void event_loop::loop()
{
    assert(!looping_);
    assert_in_loop_thread();

    TRACE << "event_loop " << this << " star looping.";
    looping_ = false;

    poll(NULL, 0, 5 * 1000);

    TRACE << "event_loop " << this << " stop looping.";
    looping_ = false;
}

} // namespace m
