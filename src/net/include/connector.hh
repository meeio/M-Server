#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "inet_address.hh"
#include "pollable.hh"
#include "timer.hh"

namespace m
{

class event_loop;

class connector
    : pollable
{
public:
    const static int RETRY_DELAY_MS = 500;

    connector(event_loop&, inet_address);
    ~connector();

/* ------------------------ CONNECTORS ----------------------- */

    void connect();
    void reconnect();
    void disconnect();



private:



};

} // namespace m

#endif // __CONNECTOR_H__