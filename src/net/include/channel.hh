#ifndef __H_CHANNEL__
#define __H_CHANNEL__

#include <functional>

#include "event_loop"

/**
 *  @channel object belongs to one @event_loop
 *  @channel object handle one fd by distributing it's callback function.
 **/
class channel
{
public:
    typedef std::function<void ()> t_callback;

    channel()
};

#endif // !__H_CHANNEL__