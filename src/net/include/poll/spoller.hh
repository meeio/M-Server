#ifndef __SPOLLER_H__
#define __SPOLLER_H__

#include <map>
#include <vector>

#include <poll.h>
#include <tuple>

#include "../poller.hh"


namespace m
{

class spoller
    : public poller
{
public:
    spoller(event_loop&);
    ~spoller();

    virtual channel_vector poll(int timeout_ms) override;
    virtual void           update_channel(channel*) override;
    virtual void           remove_channel(channel*) override;


private:
    typedef std::vector<pollfd>     pollfd_vector;
    typedef std::map<int, channel*> channel_map;

    channel_vector find_active_channel(int);

    pollfd_vector pollfds_;
    channel_map   channels_;
};

} // namespace m

#endif // __POLLER_H__