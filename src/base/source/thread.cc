#include "thread.hh"

#include <exception>
#include <iostream>

#include "current_thread.hh"
#include "logger.hh"

namespace m
{

// struct thread_contex
// {
//     thread_contex(thread::t_thread_fun tfunc,
//                   const string&        tname,
//                   int&                 ttid,
//                   m::latch&            tlatch)
//         : func(tfunc)
//         , name(tname)
//         , tid(ttid)
//         , latch(tlatch)
//     {
//     }

//     thread::t_thread_fun func;
//     const string&        name;
//     int&                 tid;
//     m::latch&            latch;
// };

int thread::created_num_ = 0;

thread::thread(thread_fun func, const string& name)
    : func_(func)
    , name_(name)
    , tid(-1)
    , latch_(1)
    , start_(false)
    , thread_handle(nullptr)
{
    if ( name_.empty() )
    {
        name_ = "[t#" + std::to_string(++created_num_) + "]";
    }
}

thread::~thread()
{
    start_ = false;
    if ( thread_handle != nullptr && thread_handle->joinable() )
    {
        thread_handle->detach();
    }
    delete thread_handle;
}

void thread::run_in_thread()
{
    start_ = true;
    tid    = current_thread::tid();
    // latch_.count_down();

    try
    {
        func_();
    }
    catch ( const std::exception& ex )
    {
        abort();
    }
    catch ( ... )
    {
        throw; // rethrow
    }
}

void thread::star()
{
    try
    {
        thread_handle = new std::thread([&] { run_in_thread(); });
        // latch_.wait();
    }
    catch ( const std::system_error& e )
    {
        std::cerr << e.what() << '\n';
    }
    catch ( const std::exception& e )
    {
        std::cerr << e.what() << '\n';
    }
}

void thread::join()
{
    thread_handle->join();
}

} // namespace m