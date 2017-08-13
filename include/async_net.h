//
// Created by qiang on 8/6/17.
//

#ifndef ASYNC_NET_H
#define ASYNC_NET_H

#include <string>
#include "async_def.h"
#include "async_event.h"
#include "async_conn.h"

class AsyncServer : public AsyncConnEvent
{
public:
    AsyncServer(const std::string& ip, int port);
    ~AsyncServer();

    // 初始化服务器
    int init(AsyncEventLoop* listen_loop);
    int init(AsyncEventLoop* listen_loop, AsyncEventLoop* recv_loop_arr[], uint32_t recv_arr_size);
    int start();

public:
    virtual int onReadable();
    virtual int onWritable(); 

private:
    AsyncEventLoop* getRecvLoop(int fd);
    int32_t addConn(int fd, AsyncConn* conn);

private:
    static int setReuseAddr(int fd, int reuse);
    static int setTCPNoDelay(int fd, int no_delay);
    static int setNonBlock(int fd, int non_block);
    int bindSocket();
    int acceptSocket();

    std::string ip_;
    int port_;
    
    static const uint32_t MAX_RECV_LOOP_NUM = 32;
    AsyncEventLoop* recv_loop_arr_[MAX_RECV_LOOP_NUM];
    uint32_t recv_loop_arr_size_;

    // TODO: manmage connection
    AsyncConn* conn_arr_[1024];
    uint32_t conn_id_;
};

#endif //ASYNC_EVENT_NET_ASYNC_EPOLL_H
