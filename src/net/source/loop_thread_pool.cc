#include "loop_thread_pool.hh"

#include <assert.h>

#include "event_loop.hh"
#include "loop_thread.hh"

namespace m
{

loop_thread_pool::loop_thread_pool(event_loop& loop)
    : master_loop_(loop)
    , started_(false)
    , thread_num_(0)
    , next_thread_idx_(0)
{
}


loop_thread_pool::~loop_thread_pool() 
{
}


void loop_thread_pool::star()
{
    master_loop_.assert_in_loop_thread();
    assert(started_ == false);

    for ( size_t i = 0; i < thread_num_; i++ )
    {
        loop_thread_ptr ploop_thread = std::make_unique<loop_thread>();
        event_loop& loop = ploop_thread->start_loop();
        loop_threads_.push_back(std::move(ploop_thread));
        loops_.push_back(&loop);
    }
}

event_loop& loop_thread_pool::get_next_loop()
{
    master_loop_.assert_in_loop_thread();

    event_loop* ret_loop = &master_loop_;
    if ( !loops_.empty() )
    {
        ret_loop = loops_[next_thread_idx_++];
        if ( next_thread_idx_ >= loops_.size() )
        {
            next_thread_idx_ = 0;
        }
    }

    return *ret_loop;
}

} // namespace m
