#include "loop_thread.hh"
#include <string>

namespace m
{

std::atomic_int loop_thread::looping_create_ = 0;

loop_thread::loop_thread()
    : ploop_()
    , thread_([&] { start_loop_in_thred(); },
              std::to_string(++looping_create_))
{
}

loop_thread::~loop_thread() 
{
    ploop_->quit();
    thread_.join();
}

event_loop& loop_thread::start_loop()
{
    assert(!thread_.started());
    thread_.star();

    {
        std::unique_lock lk(mutex_);
        cond_.wait(lk, [&] { return ploop_ != nullptr; });
    }

    return *ploop_;
}

void loop_thread::start_loop_in_thred()
{
    event_loop loop;

    {
        std::unique_lock lk(mutex_);
        ploop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();
}

} // namespace m
