#include "current_thread.hh"
#include "logger.hh"

#include <unistd.h>
#include <sys/syscall.h>

namespace m::current_thread
{

thread_local int chached_tid__ = 0;

int tid()
{
    if (chached_tid__ == 0)
        chached_tid__ = static_cast<int>(syscall(SYS_gettid));
    return chached_tid__;
}

} // namespace m::current_thread