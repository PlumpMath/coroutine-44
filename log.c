#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "log.h"

static char* log_path = NULL;
static int log_level = 0;
static int log_intval = 0;
static time_t last_time = -1;
static int log_fd= -1;

char* get_logfile_name();

char* level[] = {"DEBUG", "INFO", "WARN", "ERROR" };

void log_write(int prio, const char *file, int line, const char *fmt, ...)
{
    static char last_name[256] = {0}; 
    if (prio < log_level) return;
    struct timeval tmval;
    gettimeofday(&tmval, NULL); 
    time_t now = tmval.tv_sec;
    if ((now- last_time) > log_intval) 
    {
        char* filename = get_logfile_name();
        strncpy(last_name, filename, 256);
#ifdef test
        printf("log filename :%s \n", filename);
#endif
        if(log_fd > 0)
        {
            close(log_fd);
        }
        log_fd = open(filename, O_WRONLY|O_APPEND|O_CREAT, 0666);
        if (log_fd<0)   
        {
#ifdef test
            printf("create log failed\n");
#endif
            return; 
        }
    }
    
    static char buf[4096] = {0};
    memset(buf, 0, 4096);
    strftime(buf, 4095, "%Y-%m-%d %H:%M:%S", localtime(&now));
    int len = strlen(buf);
    snprintf(buf+len, 4094-len, ".%d ", tmval.tv_usec/1000);
    len = strlen(buf);
    snprintf(buf+len, 4094-len, "%s [%s:%d] ", level[prio], file, line);

    va_list ap;
    va_start(ap, fmt);
    len = strlen(buf);
    if(len>=4094) return;
    vsnprintf(buf+len, 4094-len, fmt, ap);
    va_end(ap);

    len = strlen(buf);
    if(len>=4094) len=4094;
    snprintf(buf+len, 4094-len, "\n\0");

    if(strlen(last_name)>0 && access(last_name, F_OK) < 0)
    {
        if(log_fd>0)close(log_fd);
        printf("log file is removede \n");
        log_fd = open(last_name, O_WRONLY|O_APPEND|O_CREAT, 0666);
    }
    len = write(log_fd, buf, strlen(buf));
#ifdef test
    if(len < 0)
    {
        printf("write log file failed, errno: %d , reason: %s\n", errno, strerror(errno));
    }
#endif
}


void log_init(char* log_dir, int intval, int level)
{
    if(access(log_dir, O_RDWR)!=0)  
    {  
        if(mkdir(log_dir, 0666)==-1)  
        {   
            printf("init log failed, log_path: %s \n", log_dir);
            exit(-1);   
        }  
    }
    log_path = log_dir;
    log_level = level;
    log_intval = intval * 60;
    last_time = -1;
}

char* get_logfile_name()
{
    static char datetime[256];
    memset(datetime, 0, 256);
    struct tm* tm_now;
    time(&last_time);
    last_time = last_time - (last_time % log_intval);
    tm_now = localtime(&last_time);
    snprintf(datetime, 256, "%s/", log_path);
    int len = strlen(datetime);
    strftime(datetime+len, 256-len, "%Y%m%d%H%M.log",tm_now);
    return datetime;

}

#ifdef test
int main(int argc, char** argv)
{
    log_init("../logs/", 1, ERROR);
    log_error("test ok");
    return 0;
}
#endif
