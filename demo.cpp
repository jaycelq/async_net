#include <stdlib.h>
#include "async_net.h"
#include "async_event.h"

int main()
{
    AsyncEventLoop event_loop;
    if (event_loop.init() != 0)
    {
        LOG_ERROR("event_loop init failed");
        exit(-1);
    }

    AsyncServer server("127.0.0.1", 8088);
    if (server.init(&event_loop) != 0)
    {
        LOG_ERROR("server init failed");
        exit(-1);
    }

    if (server.start() != 0)
    {
        LOG_ERROR("server start failed");
        exit(-1);
    }

    while(true)
    {
        event_loop.poll(-1);
    }

    return 0;
}
