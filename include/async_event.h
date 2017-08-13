//
// Created by qiang on 8/7/17.
//

#ifndef ASYNC_EVENT_H
#define ASYNC_EVENT_H

#include "async_def.h"
#include <sys/epoll.h>

class AsyncEventLoop;
class AsyncConnEvent
{
    friend class AsyncEventLoop;
public:
    AsyncConnEvent(); 
    AsyncConnEvent(int fd);
    virtual ~AsyncConnEvent();

public:
    virtual int onReadable() = 0;
    virtual int onWritable() = 0;

protected:
    int mask_;  //状态掩码，READABLE(EPOLLIN)或者WRITEABLE(EPOLLOUT)
    int fd_;    //连接句柄，用来索引连接
    AsyncEventLoop* event_loop_;
};

class AsyncEventLoop
{
public:
    AsyncEventLoop();
    ~AsyncEventLoop();

    int init();
    int fini();

    int registerEvent(AsyncConnEvent* event);
    int addConnEvent(int fd, int mask);
    int deleteConnEvent(int fd, int delmask);
    int unregisterEvent(int fd);

    int poll(uint32_t timeout_ms);
private:
    AsyncConnEvent* events[1024];

    int epfd;
};

#endif //ASYNC_EVENT_NET_ASYNC_EPOLL_H
