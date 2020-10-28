#ifndef __POLLER_H__
#define __POLLER_H__


#include <map>
#include <poll.h>
#include <vector>
#include <chrono>

#include <tuple>
#include <memory>

#include "event_loop.hh"
#include "channel.hh"

namespace m
{

class event_loop;
class channel;

class poller
{
public:
    typedef std::vector<channel*> t_channel_vector;
    typedef std::chrono::system_clock::time_point t_time;

    poller(event_loop*);
    ~poller();

    t_time poll(int timeout_ms, t_channel_vector&);

    void update_channel(channel*);

    void assert_int_loop_thread()
    {
        owner_loop_ -> assert_in_loop_thread();
    }

private:
    typedef std::vector<pollfd>     t_pollfd_vector_;
    typedef std::map<int, channel*> t_channel_map_;

    void find_active_channel(int, t_channel_vector&);

    event_loop*      owner_loop_;
    t_pollfd_vector_ pollfds_;
    t_channel_map_   channels_;
};

} // namespace m

#endif // __POLLER_H__