#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

int glb_var = 100;

void *fun1(void* arg)
{
        int i = (int)(arg);
        glb_var = i;
        printf("sub pthread %d, id: %lu, pid:%d, tid:%ld, glb_var:%d\n", i, pthread_self(), getpid(), gettid(), glb_var);
        return NULL;
}

int main(int argc, char *argv[])
{
        int i = 0;
        pthread_t pthreadid;
        for (; i < 32; i++) {
                if (pthread_create(&pthreadid, NULL, fun1, (void*)i) != 0)
                        perror("pth create err");
        }
        pthread_exit(NULL);
}