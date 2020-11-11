#ifndef __LOOP_THREAD_H__
#define __LOOP_THREAD_H__

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "event_loop.hh"
#include "thread.hh"

namespace m
{

class loop_thread
{
public:
    loop_thread();
    ~loop_thread();
    

    event_loop* start_loop();

private:
    void start_loop_in_thred();

    event_loop* ploop_;
    thread      thread_;

    std::mutex              mutex_;
    std::condition_variable cond_;

    static std::atomic_int looping_create_;
};

} // namespace m

#endif // __LOOP_THREAD_H__