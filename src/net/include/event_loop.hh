#ifndef __EVENT_LOOP__
#define __EVENT_LOOP__

#include <cassert>

#include "current_thread.hh"

namespace m
{

class event_loop
{
public:
    event_loop();

    void loop();

    void assert_in_loop_thread()
    {
        if (!is_in_loop_thread())
            assert(false);
    }

    bool is_in_loop_thread() const { return thread_id_ == m::current_thread::tid(); }

private:
    bool      looping_;
    const int thread_id_;
};

} // namespace m

#endif // !__EVENT_LOOP__