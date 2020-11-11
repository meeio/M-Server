#ifndef __POLLER_H__
#define __POLLER_H__


#include <map>
#include <poll.h>
#include <vector>
#include <chrono>

#include <tuple>
#include <memory>

#include "clock.hh"
#include "channel.hh"

namespace m
{

class event_loop;
class channel;

class poller
{
public:
    typedef std::vector<channel*> channel_vector_t;

    poller(event_loop*);
    ~poller();

    time_point_t poll(int timeout_ms, channel_vector_t&);

    void update_channel(channel*);

    void assert_int_loop_thread();

private:
    typedef std::vector<pollfd>     pollfd_vector_t_;
    typedef std::map<int, channel*> channel_map_t_;

    void find_active_channel(int, channel_vector_t&);

    event_loop*      owner_loop_;
    pollfd_vector_t_ pollfds_;
    channel_map_t_   channels_;
};

} // namespace m

#endif // __POLLER_H__