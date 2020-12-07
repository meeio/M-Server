#ifndef __POLLER_H__
#define __POLLER_H__

#include <map>
#include <vector>

#include <memory>
#include <tuple>

namespace m
{

class event_loop;
class poll_handle;

class poller
{
public:
    typedef std::unique_ptr<poller> ptr;
    typedef std::vector<std::reference_wrapper<poll_handle>>
        channel_vector;

    poller(event_loop& loop);
    ~poller();

    static ptr create_poller(event_loop&);

    virtual channel_vector poll(int timeout_ms)        = 0;
    virtual void           update_handle(poll_handle&) = 0;
    virtual void           remove_handle(poll_handle&) = 0;
    virtual poll_handle&   register_polling(int)        = 0;

    event_loop& loop() { return owner_loop_; }
    void        assert_in_loop_thread();

private:
    event_loop& owner_loop_;
};

} // namespace m

#endif // __POLLER_H__