#ifndef __POLLER_WAKER_H__
#define __POLLER_WAKER_H__


/* ----------------------------------------------------------- */
/*                      CLASS POLLER_WAKER                     */
/* ----------------------------------------------------------- */

#include "copytype.hh"
#include "poll_handler.hh"

namespace m 
{
    
class poller;
class poll_handle;

class poller_waker
    : public poll_handler
{

public:
    poller_waker(poller&);
    ~poller_waker();

    void wakeup_loop();

protected:
    virtual void handle_read(const time_point&) override;

private:
    poll_handle& phandle_;
};

} // namespace m

#endif // __POLLER_WAKER_H__