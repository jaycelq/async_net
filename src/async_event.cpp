#include "async_event.h"

AsyncConnEvent::AsyncConnEvent() : mask_(0), fd_(0), event_loop_(NULL)
{}

AsyncConnEvent::AsyncConnEvent(int fd) : mask_(0), fd_(fd), event_loop_(NULL)
{}

AsyncConnEvent::~AsyncConnEvent()
{
    if (event_loop_ != NULL)
    {
        //event_loop->deleteConnEvent(fd);
    }
}

AsyncEventLoop::AsyncEventLoop() : epfd(0)
{
    bzero(events, sizeof(events));
}

AsyncEventLoop::~AsyncEventLoop()
{
}

int AsyncEventLoop::init()
{
    epfd = epoll_create(1024);
    if (epfd == -1)
    {
        LOG_ERROR("epoll_create failed, errmsg: %s", strerror(errno));
        return AE_ERR;
    }

    return AE_OK;
}

int AsyncEventLoop::fini()
{
    return 0;
}

int AsyncEventLoop::registerEvent(AsyncConnEvent* event)
{
    if (event == NULL)
    {
        LOG_ERROR("event register is NULL");
        return AE_ERR;
    }

    if (event->fd_ < 0 || event->fd_ >= AE_MAX_EVENT_FD)
    {
        LOG_ERROR("event fd=%d is out of range.", event->fd_);
        return AE_ERR;
    }

    if (events[event->fd_] != NULL)
    {
        LOG_ERROR("event fd=%d has been registered", event->fd_);
        return AE_ERR;
    }

    if (event->mask_ != 0)
    {
        LOG_ERROR("event to register fd=%d must have mask 0", event->fd_);
        return AE_ERR;
    }

    events[event->fd_] = event;
    event->event_loop_ = this;

    LOG_INFO("registerEvent fd=%d succ.", event->fd_);

    return AE_OK;
}

int AsyncEventLoop::addConnEvent(int fd, int mask)
{
    if (fd < 0 || fd >= AE_MAX_EVENT_FD)
    {
        LOG_ERROR("addConnEvent fd=%u is out of range", fd);
        return AE_ERR;
    }

    AsyncConnEvent* event = events[fd];
    if (event == NULL)
    {
        LOG_ERROR("addConnEvent fd=%u exist", fd);
        return AE_ERR;
    }
    
    int op = event->mask_ == AE_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD; 
    struct epoll_event ee = {0};
    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(epfd, op, fd, &ee) == -1)
    {
        LOG_ERROR("epoll_ctl failed, epfd=%d op=%d fd=%d, errmsg:%s", epfd, op, fd, strerror(errno));
        return AE_ERR;
    }
    event->mask_ |= mask;

    LOG_INFO("addConnEvent fd=%d maks=%d", fd, event->mask_);
    
    return AE_OK;
}

int AsyncEventLoop::poll(uint32_t )
{
    int ret, events_num = 0;

    struct epoll_event ep_events[1024];
    ret = epoll_wait(epfd, ep_events, 1024, -1);
    if (ret > 0)
    {
        events_num = ret;
        for(int event_idx = 0; event_idx < events_num; event_idx++)
        {
            const epoll_event& e = ep_events[event_idx];
            int event_fd = e.data.fd;
            AsyncConnEvent* conn_event = events[event_fd];
            if (conn_event == NULL | conn_event->fd_ != event_fd)
            {
                LOG_FATAL("epoll error");
                continue;
            }

            if (e.events & EPOLLIN) 
            {
                conn_event->onReadable();
            }
            if (e.events & EPOLLOUT)
            {
                conn_event->onWritable();
            }
        }
    }

    return events_num;
}
