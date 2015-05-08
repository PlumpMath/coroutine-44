#include <stdlib.h>
#include <stdio.h>
#include "context.h"
#include "log.h"


#define test 
static uthread_t current = 0;
static uthread_t last = 0;
static struct uthread_struct uthread_slots[UTHREAD_MAX_NUM];

int get_upid()
{
    return current;
}
void panic(void)
{
    log_error("Panic");
    exit(EXIT_FAILURE);
}

void uthread_loop(void);
int uthread_create(uthread_t *thread, void* (*start_routine)(void*), void *arg);
void uthread_yeild(void);

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
    uthread_yeild();
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

void uthread_init(void)
{
    uthread_context_init(0);
    uthread_slots[0].status = used;
    //makecontext(&uthread_slots[0].context, main_uthread, 0);
}

void uthread_schedule(void);

void uthread_exit(void *exit_status)
{
    uthread_slots[current].exit_status = exit_status;
    uthread_slots[current].status = idle;
    if(uthread_slots[current].parent != 0)
    {
        int parent =  uthread_slots[current].parent;
        uthread_slots[parent].status  = used;
        log_debug("zzzzzzzzzzzzzzzzzzzzz");
    }
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
    int i;
    for (i = 1; i < UTHREAD_MAX_NUM; i++)
    {
        if (uthread_slots[i].status != idle)continue; 
        if (thread != NULL) *thread = i;
        uthread_context_init(i);
        uthread_slots[i].status = used;
        uthread_slots[i].parent = current;
        uthread_slots[i].func = start_routine;
        uthread_slots[i].arg = arg;
        uthread_slots[i].exit_status = 0;
        makecontext(&uthread_slots[i].context, uthread_helper, 0);
        log_info("create new uthread id:%d current:%d ", i, current);
        return 0;
    }
    log_info("create uthread failed ");
    return -1;
}

void uthread_yeild(void)
{
    if(current == 0)return ;
    uthread_slots[current].status = suspend;
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
    uthread_slots[id].status = used;
}
void uthread_loop(void)
{
    //log_debug("loop ");
    int i;
    for (i = (last + 1) % UTHREAD_MAX_NUM; i != last; i = (i + 1) % UTHREAD_MAX_NUM)
    {
        if (i != 0 && uthread_slots[i].status == used)
        {
            current = i;
            //log_info("in loop, current: %d ,last:%d  ", current, last);
            swapcontext(&uthread_slots[0].context, &uthread_slots[current].context);
            //log_debug("return to main_uthread");
//            return; 
        }
    }
    //log_debug("out loop, current: %d ,last:%d  ", current, last);
}

#ifdef test 
int main(int argc, char *argv[])
{
    log_init("./log/", 60, 0);
    uthread_t tid;
    uthread_init();
    init_listen_fd();
    epoll_init();

    main_uthread();

    return 0;
}
#endif 
