#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *strtrimr(char *pstr)  
{  
    int i;  
    i = strlen(pstr) - 1;  
    while (isspace(pstr[i]) && (i >= 0))  pstr[i--] = '\0';  
    return pstr;  
}  
char *strtriml(char *pstr)  
{  
    int i = 0,j;  
    j = strlen(pstr) - 1;  
    while (isspace(pstr[i]) && (i <= j))  i++;  
    if (i>0)  strcpy(pstr, &pstr[i]);  
    return pstr;  
}  
char *strtrim(char *pstr)  
{  
    char *p;  
    p = strtrimr(pstr);  
    return strtriml(p);  
}  

char* read_key(const char *file, char* section, char* key)
{  
    static char line[1024];  
    memset(line,0x00, 1024);
    FILE *fp;  
    fp = fopen(file,"r");  
    if(fp == NULL) 
    {
        printf("open config file:%s failed\n", file);
        return NULL;  
    }
    int flag = 0;
    if(section == NULL) section = "global";
    while(NULL != fgets(line, 1023, fp))
    {
        //char* ret = fgets(line, 1023, fp);
        //if(ret==NULL ||  strlen(line) >= 1022)
        printf("line: %s \n", line);
        if( strlen(line) >= 1022)
        {
            printf("read line failed, line:%s \n", line);
            break;
        }
        char *p = strtrim(line);  
        int len = strlen(p);  
        if(len <= 0) continue;  
        if(p[0]=='#' || p[0] == ';' || p[0] == '"')continue;     
        if(flag == 1 && p[0] == '[' )
        {
            printf("read next section \n");
            break;
        }
        if(flag == 0 && p[0] == '[' && p[len-1] == ']' )
        {
            if(strncmp(p+1, section, len-2)==0) flag = 1; 
            printf("get target section: %s\n", section);
            continue;
        }
        
        char *p2 = strchr(p, '=');  
        if(p2 == NULL) 
        {
            printf("key not split by '=', line:%s \n ", p);
            break;
        }
        *p2++ = '\0';  
        strtrim(p);
        strtrim(p2);
        if(strcmp(p, key) != 0)continue;
        fclose(fp);
        return p2;
    }  
    fclose(fp);  
    return NULL;  
}  


#ifdef TEST

int main(int argc, char** argv)
{

    char* key1 = "fuck";
    char* val1 = read_key("test.conf", NULL, key1);
    printf("read conf, key:%s, val:%s \n", key1, val1);
    char* key2= "asdfa";
    char* val2 = read_key("test.conf", "user", key2);
    printf("read conf, key:%s, val:%s \n", key2, val2);
}

#endif
