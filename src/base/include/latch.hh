#ifndef __H_LATCH__
#define __H_LATCH__

#include <condition_variable>
#include <mutex>

#include "copytype.hh"

namespace m
{

class latch
    : noncopyable
{
public:
    latch(int count);

    void wait();
    void count_down();
    int  get_count();

private:
    typedef std::unique_lock<std::mutex> t_lock_;

    int                     count_;
    mutable std::mutex      mutex_;
    std::condition_variable cond_;
};

} // namespace m

#endif // !__H_LATCH__
