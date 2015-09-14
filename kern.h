#ifndef __KERN_H__
#define __KERN_H__
struct event_private_data
{
    int fd;
    int upid;
};



int setnonblocking(int fd) ;
void init_listen_fd() ;

void epoll_init() ;
int create_work_uthread(int fd);
int active_work_uthread(int fd) ;
int epoll_loop();

int m_recv(int fd, char* buf, int len, int timeout);
int m_send(int fd, char* buf, int len, int timeout);
int m_connect(int fd, struct sockaddr* addr, int len, int timeout);

#endif
