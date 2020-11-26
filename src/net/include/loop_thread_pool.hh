#ifndef __LOOP_THREAD_POOL_H__
#define __LOOP_THREAD_POOL_H__

#include <memory>
#include <sys/types.h>
#include <vector>

namespace m
{

class event_loop;
class loop_thread;

class loop_thread_pool
{
public:
    /* --------------------- (DE)CONSTRUCTOR --------------------- */
    loop_thread_pool(event_loop&);
    ~loop_thread_pool();

    void        star();
    void        set_thread_num(int num) { thread_num_ = num; }
    event_loop& get_next_loop();

private:
    typedef std::unique_ptr<loop_thread> loop_thread_ptr;

    event_loop& master_loop_;
    bool        started_;

    std::vector<loop_thread_ptr> loop_threads_;
    std::vector<event_loop*>     loops_;
    size_t                       thread_num_;
    size_t                       next_thread_idx_;
};

} // namespace m

#endif // __LOOP_THREAD_POOL_H__