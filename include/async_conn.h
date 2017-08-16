#ifndef _ASYNC_CONN_H_
#define _ASYNC_CONN_H_

#include <string>
#include "async_event.h"

class AsyncServer;
class AsyncConn : public AsyncConnEvent
{
    friend class AsyncServer;
public:
    AsyncConn(int fd, const std::string& ip, int port);
    virtual ~AsyncConn();

    void setConnId(uint32_t conn_id);

public:
    virtual int onReadable();
    virtual int onWritable();

    AsyncServer* server;
private:
    std::string ip_;
    int port_;
    uint32_t conn_id_;


    char recv_buf[1024];    
    char send_buf[1024];
};

#endif
