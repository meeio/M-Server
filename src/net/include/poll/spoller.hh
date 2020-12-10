#ifndef __SPOLLER_H__
#define __SPOLLER_H__

#include <map>
#include <vector>

#include <tuple>

#include "../poller.hh"

struct pollfd;

namespace m
{

class spoller
    : public poller
{
public:
    spoller(event_loop&);
    ~spoller();

    virtual handle_vector poll(int timeout_ms) override;
    virtual poll_handle&  register_polling(int) override;
    virtual void          update_handle(poll_handle&) override;
    virtual void          remove_handle(poll_handle&) override;

private:
    static const int IGNORED_FD = -1;
    // typedef std::unique_ptr<poll_handle> handle_ptr;
    typedef std::vector<pollfd>        pollfd_list;
    typedef std::map<int, poll_handle> handle_list;

    handle_vector find_active_handles(int);

    pollfd_list pollfds_;
    handle_list handles_;
};

} // namespace m

#endif // __POLLER_H__