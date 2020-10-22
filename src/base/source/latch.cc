#include "latch.hh"

namespace m
{

latch::latch(int count)
    : count_(count)
{
}

void latch::wait()
{
    t_lock_ lock(mutex_);
    cond_.wait(lock, [&](){ return count_ == 0; });
}

void latch::count_down()
{
    t_lock_ lock(mutex_);
    count_ --;
    if (count_ == 0)
    {
        cond_.notify_all();
    }
}

int latch::get_count()
{
    t_lock_ lock(mutex_);
    return count_;
}

} // namespace m