#ifndef __H_CURRENT_THREAD__
#define __H_CURRENT_THREAD__

namespace m::current_thread
{

extern thread_local int chached_tid__;

int tid();
    
} // namespace m::current_thread


#endif // !__H_CURRENT_THREAD__