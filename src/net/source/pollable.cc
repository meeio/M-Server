#include "pollable.hh"
#include "event_loop.hh"

namespace m
{

void pollable::rebind_channel(int fd)
{
    owner_loop().run_in_loop(
        [&] { rebind_channel_in_loop(fd); });
}

void pollable::assert_in_loop_thread()
{
    owner_loop().assert_in_loop_thread();
}

void pollable::unwatch_this_channel()
{
    owner_loop().remove_channel(pchannel_.get());
}

bool pollable::is_in_loop_thread()
{
    return owner_loop().is_in_loop_thread();
}

void pollable::bind_this_channel_to_fd(int fd)
{

    pchannel_ = std::make_unique<channel>(owner_loop(), fd);

    pchannel_->set_read_callback(
        [&](auto&... args) { handle_read(args...); });

    pchannel_->set_write_callback(
        [&](auto&... args) { handle_write(args...); });

    pchannel_->set_close_callback(
        [&](auto&... args) { handle_close(args...); });

    pchannel_->set_err_callback(
        [&](auto&... args) { handle_error(args...); });
}


void pollable::rebind_channel_in_loop(int fd) 
{
    if ( pchannel_ != nullptr )
    {
        pchannel_->disable_all();
        unwatch_this_channel();
    }
    bind_this_channel_to_fd(fd);
}


} // namespace m
