#ifndef _ASYNC_CONN_H_
#define _ASYNC_CONN_H_

#include <string>
#include "async_event.h"

class AsyncConn : public AsyncConnEvent
{
public:
    AsyncConn(int fd, const std::string& ip, int port);
    virtual ~AsyncConn();

    void setConnId(uint32_t conn_id);

public:
    virtual int onReadable();
    virtual int onWritable();

private:
    std::string ip_;
    int port_;
    uint32_t conn_id_;

    char recv_buf[1024];    
    char send_buf[1024];
};

#endif
