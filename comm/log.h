#ifndef __LOG_H__
#define __LOG_H__
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif
#define DEBUG 0 
#define INFO  1 
#define WARN  2 
#define ERROR 3

#define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
void log_init(const char* log_dir, int intval, int level);
void log_write(int prio, const char* file, int line, const char *fmt, ...);



inline uint64_t getms(void)
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

#define log_error(fmt, args...) \
    log_write(ERROR, FILE, __LINE__, fmt, ##args)

#define log_warn(fmt, args...) \
    log_write(WARN, FILE,  __LINE__, fmt, ##args)

#define log_info(fmt, args...) \
    log_write(INFO, FILE, __LINE__, fmt, ##args)

#define log_debug(fmt, args...) \
    log_write(DEBUG, FILE, __LINE__, fmt, ##args)


#ifdef __cplusplus
}
#endif

#endif
