//
// Created by qiang on 8/7/17.
//

#ifndef ASYNC_EVENT_H
#define ASYNC_EVENT_H

class asyncConnEvent
{
private:
    int mask;  //状态掩码，READABLE(EPOLLIN)或者WRITEABLE(EPOLLOUT)
    int fd;    //连接句柄，用来索引连接

public:
    virtual int onReadable();
    virtual int onWriteable();
};

class asycEventLoop
{
private:
    asyncConnEvent events[1024];
};

#endif //ASYNC_EVENT_NET_ASYNC_EPOLL_H
