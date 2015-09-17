#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include <ucontext.h>
#include <algorithm>
#include <stdint.h>
#include <vector>
#include <string>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#define UTHREAD_MAX_NUM  256 
#define STACK_SIZE 4096
typedef int uthread_t;

struct uthread_struct
{
    int timeout;
    ucontext_t context;
    char stack[STACK_SIZE];
    void* (*func)(void *arg);
    void *arg;
    void *exit_status;
};

struct timer_info
{
    int timer_id;
    uint64_t last_time;
    int intval;
    bool is_loop;
    void* (*func)(void *arg);
    void *arg;
    
};

using namespace std;
template <typename T> class heap
{
    private:
        //声明了一个函数指针用于指向比较堆里面元素大小的函数
        typedef  bool (*COMP_FUNC)(const T&, const T&);
        COMP_FUNC _comp;
        vector<T> _data;
    public:
        heap(COMP_FUNC compare_function = NULL) :_comp(compare_function) //如果不指定这个函数指针变量的值，那么会自动调用类实现的 operator < 函数
        {
            if(NULL == _comp)
            {
                make_heap(_data.begin(), _data.end());
            }
            else
            {
                make_heap(_data.begin(), _data.end(), _comp);
            }
        }
        void add(const T& item)
        {
            //往堆里面添加元素，顺序为先添加(push_back)再重整堆结构(push_heap)
            _data.push_back(item);
            if(NULL == _comp)
            {
                push_heap(_data.begin(), _data.end());
            }
            else
            {
                push_heap(_data.begin(), _data.end(), _comp);
            }
        }
        T remove(void)
        {
            //往堆里面移除元素，顺序为先重整堆结构(pop_heap)再移除元素(pop_back)
            if(NULL == _comp)
            {
                pop_heap(_data.begin(), _data.end());
            }
            else
            {
                pop_heap(_data.begin(), _data.end(), _comp);
            }
            T removed = _data.back();
            _data.pop_back();
            return removed;
        }
        void remove(T t)
        {
            _data.erase(std::remove(_data.begin(), _data.end(), t), _data.end());
            std::sort_heap(_data.begin(), _data.end());
        }
        bool isEmpty() const
        {
            return _data.empty();
        }

        void clear()
        {
            _data.clear();
        }
};

int get_upid();
void panic(void);
void *func(void* a);
void *uthread_func(void* argc);
void main_uthread(void);
void uthread_context_init(int tid);
void uthread_init(void);
void uthread_exit(void *exit_status);
void uthread_child(void);
int uthread_create(uthread_t *thread, void* (*start_routine)(void*), void *arg);
int uthread_create_for_ready(uthread_t *thread, void* (*start_routine)(void*), void *arg);
void uthread_yeild(int ts);
void uthread_loop(void);
void uthread_resume(int);
int register_service(google::protobuf::Service* service);
google::protobuf::Service* GetServiceByName(string servicename);
void run(void);
#endif
