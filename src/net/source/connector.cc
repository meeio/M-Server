#include "connector.hh"

namespace m
{

connector::connector(event_loop&  owner_loop,
                     inet_address peer_addr)
    : pollable(owner_loop, pollable::EMPTY_FD)
{
}

} // namespace m
