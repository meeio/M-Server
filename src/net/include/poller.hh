#ifndef __POLLER_H__
#define __POLLER_H__

#include <chrono>
#include <map>
#include <poll.h>
#include <vector>

#include <memory>
#include <tuple>

#include "channel.hh"
#include "clock.hh"

namespace m
{

class event_loop;
class channel;

class poller
{
public:
    typedef std::vector<channel*> channel_vector_t;

    poller(event_loop&);
    ~poller();

    channel_vector_t poll(int timeout_ms);

    void update_channel(channel*);
    void remove_channel(channel*);

    event_loop& loop() { return owner_loop_; }
    void assert_in_loop_thread();

private:
    typedef std::vector<pollfd>     pollfd_vector_t_;
    typedef std::map<int, channel*> channel_map_t_;

    channel_vector_t find_active_channel(int);

    event_loop&      owner_loop_;
    pollfd_vector_t_ pollfds_;
    channel_map_t_   channels_;
};

} // namespace m

#endif // __POLLER_H__