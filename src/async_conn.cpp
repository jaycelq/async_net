#include "async_conn.h"
#include "async_net.h"

AsyncConn::AsyncConn(int fd, const std::string& ip, int port) : \
        AsyncConnEvent(fd), ip_(ip), port_(port), conn_id_(0)
{
}

AsyncConn::~AsyncConn()
{}

int AsyncConn::onReadable()
{
    int nread = read(fd_, recv_buf, 1024);
    LOG_INFO("read %s", recv_buf);

    close(fd_);
    server->removeConn(conn_id_);
    return 0;
}

int AsyncConn::onWritable()
{
    LOG_INFO("onWritable");
    return 0;
}

void AsyncConn::setConnId(uint32_t conn_id)
{
    conn_id_ = conn_id;
}
