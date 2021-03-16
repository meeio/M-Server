#ifndef __TIME_SERVER_H__
#define __TIME_SERVER_H__

#include "tcp_server.hh"

class time_server
{

public:
    time_server(m::event_loop&, m::inet_address);

    void star()
    {
        server_.start();
    }

private:
    void on_connection(const m::tcp_connection_ptr&);
    void on_message(const m::tcp_connection_ptr&,
                    m::buffer&, m::time_point&);

    m::event_loop& loop_;
    m::tcp_server  server_;
};

#endif // __TIME_SERVER_H__