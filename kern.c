#include <sys/epoll.h>
#include <stdlib.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "kern.h"
#include "log.h"
#include "context.h"
#include "hashmap.h"

#define MAX_EVENTS 10

hash_map fd2upid;

struct epoll_event ev, events[MAX_EVENTS];
int listen_sock,nfds, epollfd;

int setnonblocking(int fd)
{
    int iFlags;
    iFlags = fcntl(fd, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    fcntl(fd, F_SETFL, iFlags);
    return 0;
}

void init_listen_fd()
{
    listen_sock = socket(AF_INET, SOCK_STREAM,0);
    if(listen_sock < 0)
    {
        log_error("socket error");
        exit(-1);    
    }
    struct sockaddr_in sin;   
    sin.sin_family = AF_INET;   
    sin.sin_port = htons(8000); 
    sin.sin_addr.s_addr = INADDR_ANY;  
    if(bind(listen_sock, (struct sockaddr *)&sin, sizeof(sin))<0)
    {
        log_error("bind error");
        exit(-1);
    }
    if(listen(listen_sock, 1000) <0)
    {
        log_error("listen error");
        exit(-1);
    }
}
int cmp(void* key1, void* key2)
{
    return (int)key1 ==(int)key2?0:1;
}
void epoll_init()
{
    if(hmap_init(&fd2upid, cmp) < 0 )
    {
        log_error("init hash map failed");
        exit(-1);
    }
    epollfd = epoll_create(10);
    if (epollfd == -1) 
    {
        log_error("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) 
    {
        log_error("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
}


void clear_fd(int fd)
{
    ev.data.fd = fd;
    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) <0)
    {
        log_error("epoll_ctl EPOLL_CTL_DEL failed,fd:%d, erron:%d, reason:%s", fd, errno, strerror(errno));
    }
}

void register_fd(int fd, int event)
{
    log_debug("epoll_ctl register to epoll begin , fd:%d, event:%d", fd, event);
    setnonblocking(fd);
    ev.events = event ;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        if(errno == EEXIST)
        {
            if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev)<0)
            {
                log_debug("epoll_ctl mod failed, fd:%d, event:%d, errno:%d, reason:%s", fd, event, errno, strerror(errno)); 
                exit(-1);
            }
            else   
            {
                log_debug("epoll_ctl register to epoll ok, fd:%d, event:%d", fd, event);
                return;
            }
        }
        log_debug("epoll_ctl failed, fd:%d, event:%d, errno:%d, reason:%s", fd, event, errno, strerror(errno)); 
        exit(-1);
    }
    log_debug("epoll_ctl register to epoll ok, fd:%d, event:%d", fd, event);
}
int m_recv(int fd, char* buf, int len)
{
    log_debug("begin to recv, fd:%d", fd);
    register_fd(fd, EPOLLIN);
    int count = 0;
    while(len>0)
    {
        int ret = read(fd, buf+count, len);
        if(ret <= 0)
        {
            log_debug("read ret: %d, errno:%d, reason:%s ", ret, errno, strerror(errno));
            if(errno == EAGAIN) 
            { 
                log_debug("yeild uthread");
                uthread_yeild();
                log_debug("ningdegang");
                continue;
            }
            else  
            {
                log_debug("unknow error");
                return -1;
            }
        } 
        len -= ret;
        count += ret;
    }
}
int m_send(int fd, char* buf, int len)
{
    register_fd(fd, EPOLLOUT);
    int count = 0;
    while(len>0)
    {
        int ret = write(fd, buf+count, len);
        if(ret == 0) uthread_yeild();
        if(ret < 0)
        {
            if(errno == EAGAIN)uthread_yeild();
            else return -1;
        }
        len -= ret;
        count += ret;
    }
}
void sockaddr_init(struct sockaddr_in* in, char* ip, int port)
{
    memset(in, 0x00, sizeof(struct sockaddr_in));
    in->sin_family = AF_INET;
    in->sin_port = htons(port);
    inet_pton(AF_INET, ip, &in->sin_addr);
}
int m_connect(int fd, struct sockaddr* addr, int len)
{   
    setnonblocking(fd);
    log_debug("begin to connect to other server");
    time_t begin = time(NULL);
    int n = connect(fd, addr, len);
    if(n < 0 && errno != EINPROGRESS)
    {
        log_error("connect failed, fd:%d, errno:%d , reason:%s", fd, errno, strerror(errno));
        return -1;
    }
     
    register_fd(fd, EPOLLOUT);
    log_info("connect is in progress");
    uthread_yeild();
    time_t end = time(NULL);
    if((end-begin)> 1) return -1;
}


void* work(void* args)
{
    log_info("begin work");
    char buf[128];
    int len = 128;
    int fd= (int)args;
    if(hmap_insert(&fd2upid, fd, get_upid()) <0)
    {
        log_error("insert fd to epoll map failed");
        return -1;
    }
    m_recv(fd, buf, len);
    buf[127] = 0;
    log_info("recv client data: %s", buf);
    int sub_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(hmap_insert(&fd2upid, sub_fd, get_upid()) <0)
    {
        log_error("insert fd to epoll map failed");
        return -1;
    }
    int addr_len = sizeof(struct sockaddr_in); 
    struct sockaddr_in in;
    sockaddr_init(&in, "127.0.0.1", 8001);
    m_connect(sub_fd, (struct sockaddr*)&in, addr_len);
    m_send(sub_fd, buf, len);
    m_recv(sub_fd, buf, len);
    m_send(fd, buf, len);
    log_info("recv from other server: %s", buf);
    clear_fd(sub_fd);
    close(sub_fd);
    hmap_remove(&fd2upid, sub_fd);
    clear_fd(fd);
    close(fd);
    hmap_remove(&fd2upid, fd);
    buf[127] = 0;
    return NULL;
}
int create_work_uthread(int fd)
{
    if(uthread_create(NULL, work, (void*) fd)<0)
    {
        return -1;
    }
}

int  active_work_uthread(int fd)
{
    int upid = (int)hmap_find(&fd2upid, fd);
    log_debug("resume pid: %d", upid);
    uthread_resume(upid); 
    return 0;
}


int epoll_loop()
{
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
        log_error("epoll_pwait");
        exit(-1);
    }

    int n;
    for (n = 0; n < nfds; ++n)
    {
        if (events[n].data.fd == listen_sock) 
        {
            struct sockaddr_in client_addr; 
            int addrlen = sizeof(client_addr);
            
            int conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addrlen);
            if (conn_sock == -1) 
            {
                log_error("accept");
                exit(-1);
            }
            log_debug("new client connect, fd:%d", conn_sock);
            create_work_uthread(conn_sock);
        } 
        else 
        {
            log_debug("new data coming, fd:%d", events[n].data.fd);
            active_work_uthread(events[n].data.fd);
        }
    }
    return nfds;
}
