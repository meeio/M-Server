#include "channelable.hh"
#include "event_loop.hh"

namespace m
{

   
void channelable::assert_in_loop_thread() 
{
    loop()->assert_in_loop_thread(); 
}

void channelable::unwatch_this_channel() 
{
    loop()->remove_channel(&channel_); 
}


} // namespace m
