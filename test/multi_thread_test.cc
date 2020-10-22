
#include "logger.hh"
#include "thread.hh"

#include <unistd.h>


#include <poll.h>

void func()
{
    poll(NULL, 0, 2000);
}

void func2()
{
    poll(NULL, 0, 5000);
}

int main(int argc, char const *argv[])
{
    DEBUG_MODE;
    
    m::thread a(func);
    m::thread b(func2);

    a.star();
    b.star();

    a.join();
    b.join();

    // usleep(100000);

    return 0;
}
