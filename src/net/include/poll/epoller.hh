#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include "../poller.hh"

namespace m
{

/* ----------------------------------------------------------- */
/*                        CLASS EPOLLER                        */
/* ----------------------------------------------------------- */

class event_loop;

class epoller
    : public poller
{
public:
    /* --------------------- (DE)CONSTRUCTOR --------------------- */

    epoller();
    ~epoller();

    /* ---------------------- POLL FUNCTION ---------------------- */
    virtual channel_vector poll(int timeout_ms);
    virtual void           update_channel(channel*);
    virtual void           remove_channel(channel*);

private:

    static const int INIT_EVENT_LIST_SIZE = 32;

    typedef std::vector<struct epoll_event> event_list;

    event_list events_;

    int poller_fd_;
};

} // namespace m
#endif // __EPOLLER_H__