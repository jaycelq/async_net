#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "async_net.h"

AsyncServer::AsyncServer(const std::string& ip, int port) :         \
    AsyncConnEvent(), ip_(ip), port_(port), recv_loop_arr_size_(0), \
    conn_id_(0)
{
    bzero(recv_loop_arr_, sizeof(recv_loop_arr_));
}

AsyncServer::~AsyncServer()
{}

int AsyncServer::init(AsyncEventLoop* listen_loop)
{
    AsyncEventLoop* recv_loop_arr[] = {NULL};
    return init(listen_loop, recv_loop_arr, 0);
}

int AsyncServer::init(AsyncEventLoop* listen_loop, AsyncEventLoop* recv_loop_arr[], uint32_t recv_arr_size)
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1)
    {
        LOG_ERROR("create socket failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

	if (setNonBlock(fd_, 1) != 0)
    {
        LOG_ERROR("set setNonBlock failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    if (setReuseAddr(fd_, 1) != 0)
    {
        LOG_ERROR("set setReuseAddr failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    if (listen_loop == NULL)
    {
        LOG_ERROR("listen_loop is NULL");
        return AE_ERR;
    }

    event_loop_ = listen_loop;

    if (recv_arr_size > MAX_RECV_LOOP_NUM)
    {
        LOG_ERROR("recv_arr_size=%u exceed limit", recv_arr_size);
        return AE_ERR;
    }

    if (recv_arr_size > 0)
    {
        for (uint32_t idx = 0; idx < recv_arr_size; idx++)
        {
            AsyncEventLoop* recv_loop = recv_loop_arr[idx];
            if (recv_loop == NULL)
            {
                LOG_ERROR("recv_loop idx=%u is NULL", idx);
                return AE_ERR;
            }

            recv_loop_arr_[idx] = recv_loop;
        }
        recv_loop_arr_size_ = recv_arr_size;
    }
    else
    {
        recv_loop_arr_[0] = listen_loop;
        recv_loop_arr_size_ = 1;
    }
    
    return 0;
}

int AsyncServer::setReuseAddr(int fd, int reuse)
{
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        LOG_ERROR("setsockopt SO_REUSEADDR failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int AsyncServer::setTCPNoDelay(int fd, int no_delay)
{
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) == -1)
    {
        LOG_ERROR("setsockopt TCP_NODELAY failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }
    return AE_OK;
}

int AsyncServer::setNonBlock(int fd, int non_block)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) == -1) {
	    LOG_ERROR("setNonBlock fcntl failed, errmsg: %s", strerror(errno));	
        return AE_ERR;
    }

    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
	    LOG_ERROR("setNonBlock fcntl failed, errmsg: %s", strerror(errno));	
        return AE_ERR;
    }
    return AE_OK;
}

int AsyncServer::bindSocket()
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);

    if (bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        LOG_ERROR("bind failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int AsyncServer::acceptSocket()
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;

    clifd = accept(fd_, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (clifd == -1)
    {
        LOG_ERROR("accpet connection failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    if (setNonBlock(clifd, 1) != 0)
    {
        LOG_ERROR("setNonBlock failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    if (setTCPNoDelay(clifd, 1) != 0)
    {
        LOG_ERROR("setTCPNoDelay failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    AsyncConn* conn = new AsyncConn(clifd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    if (conn == NULL) 
    {
        LOG_ERROR("new AsyncConn failed");
        return AE_ERR;
    }

    if (conn_id_ == 0)
    {
        conn_id_++;
    }

    conn->setConnId(conn_id_);

    addConn(clifd, conn);

    return 0;
}

int AsyncServer::addConn(int fd, AsyncConn* conn)
{
    if (conn == NULL)
    {
        LOG_ERROR("Connetion is NULL");
        return AE_ERR;
    }

    if (conn_arr_[fd] != NULL)
    {
        LOG_ERROR("Connection exist, fd=%d", fd);
        return AE_ERR;
    }

    conn_arr_[fd] = conn;

    AsyncEventLoop* event_loop = getRecvLoop(fd);
    if (event_loop == NULL)
    {
        LOG_ERROR("event_loop is NULL, fd=%u", fd);
        return AE_ERR;
    }

    if (event_loop->registerEvent(conn) != 0)
    {
        LOG_ERROR("registerEvent failed, fd=%u", fd);
        return AE_ERR;
    }

    if (event_loop->addConnEvent(fd, AE_READABLE) != 0)
    {
        LOG_ERROR("addConnEvent failed, fd=%u", fd);
        return AE_ERR;
    }

    return AE_OK;
}

AsyncEventLoop* AsyncServer::getRecvLoop(int )
{
    return event_loop_;
}

int AsyncServer::onReadable()
{
    acceptSocket();
    return 0;
}

int AsyncServer::onWritable()
{
    return 0;
}

int AsyncServer::start()
{
    if (bindSocket() != 0)
    {
        LOG_ERROR("bindSocket failed, errmsg: %s", strerror(errno));
        return -1;
    }

    if (event_loop_->registerEvent(this) != 0)
    {
        LOG_ERROR("server registerEvent failed.");
        return -1;
    }

    if (event_loop_->addConnEvent(fd_, AE_READABLE) != 0)
    {
        LOG_ERROR("server addConnEvent failed.");
        return -1;
    }

    if (listen(fd_, 10) == -1)
    {
        LOG_ERROR("listen failed, errmsg: %s", strerror(errno));
        return -1;
    }

    return 0;
}
