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
static list<int> time_list;


static list<timer_info> timer_list;


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

void uthread_child(void)
{
    void* status = uthread_slots[current].func( uthread_slots[current].arg);
    uthread_exit(status);
    log_debug("uthread id:%d exit ", current);
}


int create_timer(void* (*func)(void*), int time_intval, bool is_loop)
{
    timer_info  timer;
    timer.func = func;
    timer.last_time = getms();
    timer.intval = time_intval;
    timer.is_loop = is_loop; 
    timer_list.push_back(timer);
}

int uthread_create_for_ready(uthread_t *thread, void* (*start_routine)(void*), void *arg)
{
    if(idle_list.size() == 0)return -1;
    int i = idle_list.front();
    idle_list.pop_front();
    ready_list.push_back(i);
    if (thread != NULL) *thread = i;
    uthread_context_init(i);
    uthread_slots[i].func = start_routine;
    uthread_slots[i].arg = arg;
    uthread_slots[i].exit_status = 0;
    makecontext(&uthread_slots[i].context, uthread_child, 0);
    log_info("create new uthread id:%d current:%d ", i, current);
    return i;
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
    makecontext(&uthread_slots[i].context, uthread_child, 0);
    log_info("create new uthread id:%d current:%d ", i, current);
    return i;
}

void uthread_yeild(int ts)
{
    if(current == 0)return ;
    //time_list.add(current);
    if(ts <= 0) ts = 5000;
    uthread_slots[current].timeout = ts + getms();
    bool is_insert = false;
    for(list<int>::iterator it = time_list.begin(); it != time_list.end(); it++)
    {
        if(uthread_slots[*it].timeout > uthread_slots[current].timeout)
        {
            time_list.insert(it, current);
            is_insert = true;
            break;
        } 
    }
    if(!is_insert)time_list.push_back(current);
    ready_list.remove(current);
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
    time_list.remove(id);
}
void uthread_loop(void)
{
    log_debug("begin runing uthread loop");
    while(ready_list.size() > 0)
    {
        int i = ready_list.front();
        ready_list.pop_front();
        current = i;
        log_debug("in loop, switch to: %d", current);
        swapcontext(&uthread_slots[0].context, &uthread_slots[current].context);
        //log_debug("return to main_uthread");
    }
    uint64_t  t = getms();
    while(time_list.size() > 0) 
    {
        int i = time_list.front();
        if(uthread_slots[i].timeout> t)break;
        time_list.pop_front();
        current = i;
        log_debug("in loop, switch to: %d", current);
        swapcontext(&uthread_slots[0].context, &uthread_slots[current].context);
    }
    for(list<timer_info>::iterator it = timer_list.begin(); it != timer_list.end(); it++)
    {
        if((it->last_time + it->intval) <t )
        {
            uthread_create_for_ready(NULL, it->func, it->arg);
            if(it->is_loop)
            {
                it->last_time = t;
            }
            else
            {
                timer_list.erase(it);
            }
        }
    }

    log_debug("end running uthread loop");
}

void* t(void* args)
{
    log_debug("shit shit shit time loop: %llu",  getms());
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
    create_timer(t, 200, true);
    main_uthread();

    return 0;
}
#endif 
