#include "connector.hh"
#include "event_loop.hh"
#include "logger.hh"
#include "poll_handle.hh"
#include "sys_fd.hh"

namespace m
{

connector::connector(event_loop&  owner_loop,
                     inet_address peer_addr)
    : loop_(owner_loop)
    , poll_hd_(nullptr)
    , state_(state::disconnected)
    , psocket_(nullptr)
    , peer_addr_(peer_addr)
    , retry_delay_ms(RETRY_DELAY_MS)
    , retry_timer_(nullptr)
{
}

/* ----------------------------------------------------------- */
/*                          CONNECTORS                         */
/* ----------------------------------------------------------- */

void connector::connect()
{
    loop_.run_in_loop(
        [&] { connect_in_loop(); });
}

connector::~connector()
{
    psocket_ = nullptr;
}

void connector::reconnect()
{
    assert(false);
}

void connector::wait_writable()
{
    set_state(state::connecting);
    assert(!poll_hd_);
    poll_hd_ = &loop_.register_polling(psocket_->fd());
    poll_hd_->bind_handler(*this);
    poll_hd_->enable_writing();
}

void connector::stop()
{
    set_state(state::stop);
    retry_timer_->cancel_timer();
}

/* ----------------------------------------------------------- */
/*                        EVENT HANDLERS                       */
/* ----------------------------------------------------------- */

void connector::handle_write(const time_point&)
{
    if ( state_ == state::connecting )
    {
        int err = psocket_->get_error();
        if ( err )
        {
            WARN << "so_error " << err;
        }
        else
        {
            if ( state_ != state::stop && new_connection_cb_ )
            {
                poll_hd_->disable_all();
                new_connection_cb_(std::move(*psocket_));
            }
            psocket_.release();
        }
    }
}

void connector::handle_error(const time_point&)
{
    poll_hd_->disable_all();
    loop_.queue_in_loop([&, p = poll_hd_]() {
        loop_.remove_poll_handle(*p);
    });
    poll_hd_ = nullptr;

    int err = psocket_->get_error();
    psocket_.release();

    ERR << std::strerror(err);

    retry_connect();
}

/* ----------------------------------------------------------- */
/*                      INLOOP_CONNECTORS                      */
/* ----------------------------------------------------------- */

void connector::connect_in_loop()
{
    loop_.assert_in_loop_thread();

    psocket_ = std::make_unique<socket>();

    int connect_err = psocket_->connect(peer_addr_);
    switch ( connect_err )
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        wait_writable();
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        psocket_.release();
        retry_connect();
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        psocket_.release();
        break;

    default:
        ERR << "Unexpected error in Connector::startInLoop ";
        psocket_.release();
        break;
    }

    // int ret = sock.conn
}

void connector::retry_connect()
{
    psocket_ = nullptr;

    if ( state_ != state::stop )
    {
        set_state(state::disconnected);
        INFO << "retry connect to " << peer_addr_.adress()
             << " in " << retry_delay_ms << " millisencods";
        auto rtimer = loop_.run_after(
            [&]() { connect_in_loop(); }, retry_delay_ms);
        retry_timer_   = &rtimer;
        retry_delay_ms = std::min(retry_delay_ms * 2, 3000);
    }
}

} // namespace m