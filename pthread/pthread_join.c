#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

struct student {
        int id;        
};

int glb_var = 100;

void *fun1(void* arg)
{
        int i = (int)(arg);
        printf("sub pthread %d, id: %lu, pid:%d, tid:%ld, glb_var:%d\n", i, pthread_self(), getpid(), gettid(), glb_var);
        
        struct student *t = malloc(sizeof(struct student *));
        t->id = i;
        return (void *)t;
}

int main(int argc, char *argv[])
{
        int i = 0;
        printf("sizeof strstu:%lu,sizeof(str stu*):%lu\n", sizeof(struct student), sizeof(struct student*));
        printf("sizeof(int):%lu, sizeofint*:%lu\n", sizeof(int),sizeof(int*));
        pthread_t pthreadid;
        struct student *ret;
        for (; i < 5; i++) {
                if (pthread_create(&pthreadid, NULL, fun1, (void*)i) != 0)
                        perror("pth create err");
                pthread_join(pthreadid, (void**)(&ret));
                printf("join %d thread:success,studentID:%d\n", i, ret->id);
                free(ret);
        }
        return 0;
}
