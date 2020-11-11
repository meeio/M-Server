#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"

#include <unistd.h>

void new_connection(int sfd, const m::inet_address& peer_addr)
{
    INFO << "new conn from " << peer_addr
         << " is opened in " << sfd;
    ::write(sfd, "how are you?\n", 13);
}

int main(int argc, const char** argv)
{

    TRACE_MODE;

    DEBUG << "main thred: pid = " << getpid()
          << ", tid = " << m::current_thread::tid();

    m::inet_address listen_addr(12345);
    m::event_loop   loop;

    m::acceptor acceptor(&loop, listen_addr);
    acceptor.set_new_conn_callback(new_connection);
    acceptor.listen();

    loop.loop();
}