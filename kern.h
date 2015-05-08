

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
