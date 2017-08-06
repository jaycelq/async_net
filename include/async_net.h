//
// Created by qiang on 8/6/17.
//

#ifndef ASYNC_EVENT_NET_ASYNC_EPOLL_H
#define ASYNC_EVENT_NET_ASYNC_EPOLL_H

 //TODO:implement RingBuffer

typedef std::string RingBuffer;

class asyncConnEvent
{
private:
    int mask;  //状态掩码，READABLE(EPOLLIN)或者WRITEABLE(EPOLLOUT)
    int fd;    //连接句柄，用来索引连接

public:
    virtual int onReadable();
    virtual int onWriteable();
};

class asyncConn
{
private:
    int fd;         //连接句柄
    int state;      //连接状态

    RingBuffer recv_buf;    
    RingBuffer send_buf;
};

class asycConnMgr
{
private:
    asyncConn conn_vec_[1024];  //TODO:fix max connection
};

#endif //ASYNC_EVENT_NET_ASYNC_EPOLL_H
