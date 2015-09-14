#include <stdlib.h>
#include <stdio.h>
#include <list>
#include "context.h"
#include "kern.h"
#include "log.h"

using namespace std;

#define test 
static uthread_t current = 0;
static uthread_t last = 0;
static struct uthread_struct uthread_slots[UTHREAD_MAX_NUM];
static list<int> idle_list;
static list<int> ready_list;
static list<int> wait_list;


int get_upid()
{
    return current;
}
void panic(void)
{
    log_error("Panic");
    exit(EXIT_FAILURE);
}


//int  uthread_create(uthread_t *thread, void* (*start_routine)(void*), void *arg);

void *func(void* a)
{
    log_debug("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY");
}
void *uthread_func(void* argc)
{
    log_debug("now create child uthread, current: %d ", current);
    if(uthread_create(NULL, func,  0)<0)
    {
        return NULL;
    }
    uthread_yeild(0);
    log_debug("2, last: %d , current: %d ", last, current);

//  uthread_yeild();
    log_debug("3, agin last: %d , current: %d ", last, current);
}

void main_uthread(void)
{
    int count = 0;
    while(1)
    {
        epoll_loop();
        //check_timeout();
        //log_debug("create parent uthread, current: %d, ", current);
        //uthread_create(NULL, uthread_func, NULL);
        uthread_loop();
        //log_debug("count: %d ", count++);
    }
}

void uthread_context_init(int tid)
{
    getcontext(&uthread_slots[tid].context);
    uthread_slots[tid].context.uc_stack.ss_sp = uthread_slots[tid].stack;
    uthread_slots[tid].context.uc_stack.ss_size = sizeof(uthread_slots[tid].stack);
    uthread_slots[tid].context.uc_link = &uthread_slots[0].context;
}

void main_uthread_init(void)
{
    uthread_context_init(0);
    makecontext(&uthread_slots[0].context, main_uthread, 0);
}
void child_uthread_init(void)
{
    for(int i=1; i<UTHREAD_MAX_NUM; i++)
    {
        uthread_context_init(i);
        idle_list.push_back(i);
    } 
    
}

void uthread_schedule(void);

void uthread_exit(void *exit_status)
{
    uthread_slots[current].exit_status = exit_status;
    idle_list.push_back(current);
    ready_list.remove(current);
    current = 0;
    log_debug("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    
}

void uthread_helper(void)
{
    void* status = uthread_slots[current].func( uthread_slots[current].arg);
    uthread_exit(status);
    log_debug("uthread id:%d exit ", current);
}

int uthread_create(uthread_t *thread, void* (*start_routine)(void*), void *arg)
{
    if(idle_list.size() == 0)return -1;
    int i = idle_list.front();
    idle_list.pop_front();
    wait_list.push_back(i);
    if (thread != NULL) *thread = i;
    uthread_context_init(i);
    uthread_slots[i].func = start_routine;
    uthread_slots[i].arg = arg;
    uthread_slots[i].exit_status = 0;
    makecontext(&uthread_slots[i].context, uthread_helper, 0);
    log_info("create new uthread id:%d current:%d ", i, current);
    return i;
}

void uthread_yeild(int ts)
{
    if(current == 0)return ;
    wait_list.push_back(current);
    ready_list.remove(current);
    uthread_slots[current].timeout = ts;
    log_debug("suspend thread: %d,  running thread:0", current);
    last = current;
    current = 0;
    swapcontext(&uthread_slots[last].context, &uthread_slots[0].context);
}
void uthread_resume(int id)
{
    if(id == current)   
    {
        log_debug("should not resume current thread: %d", id);
        return;
    }
    log_debug("resume uthread:%d" , id);
    ready_list.push_back(id);
    wait_list.remove(id);
}
void uthread_loop(void)
{
    log_debug("begin runing uthread loop");
    while(ready_list.size() > 0)
    {
        int i = ready_list.front();
        //ready_list.pop_front();
        current = i;
        log_debug("in loop, switch to: %d", current);
        swapcontext(&uthread_slots[0].context, &uthread_slots[current].context);
        //log_debug("return to main_uthread");
    }
    log_debug("end running uthread loop");
}

#ifdef test 
int main(int argc, char *argv[])
{
    log_init("./log/", 60, 0);
    uthread_t tid;
    main_uthread_init();
    child_uthread_init();
    init_listen_fd();
    epoll_init();
    main_uthread();

    return 0;
}
#endif 
