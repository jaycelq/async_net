//
// Created by qiang on 8/6/17.
//

#ifndef ASYNC_NET_H
#define ASYNC_NET_H

#include <string>
#include "async_def.h"

 //TODO:implement RingBuffer
typedef std::string RingBuffer;

class AsyncConn
{
private:
    int fd;         //连接句柄
    int state;      //连接状态

    RingBuffer recv_buf;    
    RingBuffer send_buf;
};

class AsyncServer
{
public:
    AsyncServer(char* ip, int port) : ip(ip), port(port), fd(0) {}
    ~AsyncServer() {}

    // 初始化服务器
    int init();
    int start();

private:
    int setReuseAddr();
    int bindSocket();

    std::string ip;
    int port;
    int fd;
};

#endif //ASYNC_EVENT_NET_ASYNC_EPOLL_H
