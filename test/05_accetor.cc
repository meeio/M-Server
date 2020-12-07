#include "acceptor.hh"
#include "event_loop.hh"
#include "logger.hh"

#include <unistd.h>

void new_connection(m::socket socket, const m::inet_address& peer_addr)
{
    INFO << "new conn from " << peer_addr
         << " is opened in " << socket.fd();
    ::write(socket.fd(), "how are you?\n", 13);
}

int main(int, const char**)
{

    TRACE_MODE;

    uint16_t listen_port = 12345;

    DEBUG << "main thred: pid = " << getpid()
          << ", tid = " << m::current_thread::tid();

    m::inet_address listen_addr(LOCAL, listen_port);
    m::event_loop   loop;

    m::acceptor acceptor(loop, listen_addr);
    acceptor.set_new_conn_callback(new_connection);
    acceptor.listen();

    loop.loop();
}