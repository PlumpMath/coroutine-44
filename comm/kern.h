#ifndef __KERN_H__
#define __KERN_H__
struct event_private_data
{
    int fd;
    int upid;
};



int setnonblocking(int fd) ;
void init_listen_fd(int port) ;

void epoll_init() ;
int create_work_uthread(int fd);
int active_work_uthread(int fd) ;
int epoll_loop();


void sockaddr_init(struct sockaddr_in* in, const char* ip, int port);

int m_recv(int fd, char* buf, int len, int timeout);
int m_send(int fd, char* buf, int len, int timeout);
int m_connect(const char* ip, int port, int timeout);
int m_close(int fd);

#endif
