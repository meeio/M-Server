#include "pollee.hh"
#include "event_loop.hh"

namespace m
{

pollee::pollee(event_loop& loop,
               pollable*   ppollable,
               int         fd)
    : loop_(loop)
    , pollable_(ppollable)
    , channel_(loop_, fd)
{
    channel_.set_read_callback(
        [&](auto&... args) { pollable_->handle_read(args...); });

    channel_.set_write_callback(
        [&](auto&... args) { pollable_->handle_write(args...); });

    channel_.set_close_callback(
        [&](auto&... args) { pollable_->handle_close(args...); });

    channel_.set_err_callback(
        [&](auto&... args) { pollable_->handle_error(args...); });
}

pollee::~pollee()
{
    channel_.disable_all();
    loop_.remove_channel(&channel_);
}

} // namespace m
