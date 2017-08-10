#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "async_net.h"

int AsyncServer::init()
{
    fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if (fd == -1)
    {
        LOG_ERROR("create socket failed, errmsg: %s", strerror(errno));
        return -1;
    }

    if (setReuseAddr() != 0)
    {
        LOG_ERROR("set setReuseAddr failed, errmsg: %s", strerror(errno));
        return -1;
    }
    
    return 0;
}

int AsyncServer::setReuseAddr()
{
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        LOG_ERROR("setsockopt SO_REUSEADDR failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int AsyncServer::bindSocket()
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        LOG_ERROR("bind failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int AsyncServer::start()
{
    if (bindSocket() != 0)
    {
        LOG_ERROR("bindSocket failed, errmsg: %s", strerror(errno));
        return -1;
    }

    if (listen(fd, 10) == -1)
    {
        LOG_ERROR("listen failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}
