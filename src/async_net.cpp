#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "async_net.h"

int AsyncServer::createSocket()
{
    fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if (fd == -1)
        
}
