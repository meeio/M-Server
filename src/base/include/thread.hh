#ifndef __H_THREAD__
#define __H_THREAD__

#include <functional>
#include <string>
#include <thread>

#include "latch.hh"

using std::string;

namespace m
{

class thread
{
public:
    typedef std::function<void()> t_thread_fun;

    thread(t_thread_fun, const string& name = string());
    ~thread();

    void star();
    void join();
    
    inline bool started() { return start_; }

private:
    typedef std::thread t_thread_handel;

    void run_in_thread();

    int              tid;
    bool             start_;
    string           name_;
    t_thread_fun     func_;
    latch            latch_;
    t_thread_handel* thread_handle;

    static int created_num_;
};

} // namespace m

#endif // !__H_THREAD__