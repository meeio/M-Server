#ifndef __POLLER_WAKER_H__
#define __POLLER_WAKER_H__


/* ----------------------------------------------------------- */
/*                      CLASS POLLER_WAKER                     */
/* ----------------------------------------------------------- */

#include "pollee.hh"
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

private:
    pollee pollee_;
};

} // namespace m

#endif // __POLLER_WAKER_H__