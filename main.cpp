#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8787
#define LISTENQ 10
#define EPOLLEVENTS 100
#define MAXSIZE 1024
#define FDSIZE 1000

static int socket_bind(const char *ip, int port)
{
    int listen_fd;
    struct sockaddr_in servaddr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket error");
        exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind error: ");
        exit(1);
    }

    return listen_fd;
}

static void add_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

static void del_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

static void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}


static void handle_accept(int epollfd, int listenfd)
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;

    clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (clifd == -1)
    {
        perror("accept error");
    }
    else
    {
        printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

        add_event(epollfd, clifd, EPOLLIN);
    }
}

static void do_read(int epollfd, int fd, char *buf)
{
    int nread;
    nread = read(fd, buf, MAXSIZE);
    if (nread == -1)
    {
        perror("read error");
        close(fd);
        del_event(epollfd, fd, EPOLLIN);
    }
    else if (nread == 0)
    {
        fprintf(stderr, "client close.\n");
        close(fd);
        del_event(epollfd, fd, EPOLLIN);
    }
    else
    {
        printf("read message is : %s\n", buf);
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

static void do_write(int epollfd, int fd, char *buf)
{
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1)
    {
        perror("write error:");
        close(fd);
        del_event(epollfd, fd, EPOLLOUT);
    }
    else
        modify_event(epollfd, fd, EPOLLIN);
    memset(buf, 0, MAXSIZE);
}

static void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf)
{
    int i;
    int fd;

    for (i = 0; i < num; i++)
    {
        fd = events[i].data.fd;
        if (fd == listenfd && (events[i].events & EPOLLIN))
            handle_accept(epollfd, listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd, fd, buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd, fd, buf);
    }
}

static void do_epoll(int listen_fd)
{
    int epoll_fd;
    struct epoll_event events[EPOLLEVENTS];

    int ret;
    char buf[MAXSIZE];

    bzero(buf, sizeof(buf));
    epoll_fd = epoll_create(FDSIZE);
    add_event(epoll_fd, listen_fd, EPOLLIN);

    for (;;)
    {
        ret = epoll_wait(epoll_fd, events, EPOLLEVENTS, -1);
        handle_events(epoll_fd, events, ret, listen_fd, buf);
    }

}

int main()
{
    int listen_fd;
    listen_fd = socket_bind(IPADDRESS, PORT);
    listen(listen_fd, LISTENQ);
    do_epoll(listen_fd);
    return 0;
}