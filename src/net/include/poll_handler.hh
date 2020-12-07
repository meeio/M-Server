#ifndef __POLL_HANDLER_H__
#define __POLL_HANDLER_H__

#include "clock.hh"
#include "logger.hh"
#include <assert.h>

namespace m
{

class poll_handler
{
public:
    virtual void handle_read(const time_point&)
    {
        INFO << "assert false";
        assert(false && "not implement!");
    };

    virtual void handle_write(const time_point&)
    {
        INFO << "assert false";
        assert(false && "not implement!");
    };

    virtual void handle_close(const time_point&)
    {
        INFO << "assert false";
        assert(false && "not implement!");
    };

    virtual void handle_error(const time_point&)
    {
        INFO << "assert false";
        assert(false && "not implement!");
    };
};

} // namespace m

#endif // __POLL_HANDLER_H__