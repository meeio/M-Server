#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"

namespace m
{

acceptor::acceptor(event_loop* loop, const inet_address& inet_addr) 
    : loop_(loop)
    , accept_socket_()
    , accept_channel_(loop, accept_socket_.fd())
    , is_listing(false)
{
    accept_socket_.bind(inet_addr);
    
    accept_channel_.set_read_callback([&]{
        loop_->assert_in_loop_thread();
        
        auto[new_conn_addr, new_conn_fd] = accept_socket_.accept();
        if (new_conn_fd >= 0)
        {
            if (new_conn_cb_)
            {
                new_conn_cb_(new_conn_fd, new_conn_addr);
            }
            else
            {
                ERR << "no cb is set";
            }
        }
    });
}


void acceptor::listen() 
{
    loop_->assert_in_loop_thread();
    is_listing = true;
    accept_socket_.listen();
    accept_channel_.enable_reading();
}


void acceptor::set_new_conn_callback(new_coon_callback_t cb) 
{
    new_conn_cb_ = cb;
}


} // namespace m
