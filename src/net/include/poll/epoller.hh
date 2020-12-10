#ifndef __SPOLLER_H__
#define __SPOLLER_H__

#include <map>
#include <vector>

#include <tuple>

#include "../poller.hh"

struct epoll_event;

namespace m
{

class epoller
    : public poller
{
public:
    epoller(event_loop&);
    ~epoller();

    virtual handle_vector poll(int timeout_ms) override;
    virtual poll_handle&  register_polling(int) override;
    virtual void          update_handle(poll_handle&) override;
    virtual void          remove_handle(poll_handle&) override;

private:
    const int INIT_RETRIVE_NUM = 1;
    const int epoll_fd_;
    std::vector<epoll_event> retrive_events_;
};

} // namespace m

#endif // __POLLER_H__