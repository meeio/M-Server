#ifndef __POLLER_WAKER_H__
#define __POLLER_WAKER_H__


/* ----------------------------------------------------------- */
/*                      CLASS POLLER_WAKER                     */
/* ----------------------------------------------------------- */

#include "pollable.hh"
#include "copytype.hh"


namespace m
{
    
class poller;

class poller_waker
    : public pollable
{

public:
    poller_waker(poller&);
    ~poller_waker();

    void wakeup_loop();

protected:
    virtual void handle_read(const time_point&) override;
    /* data */
};

} // namespace m

#endif // __POLLER_WAKER_H__