#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

void *fun1(void* arg)
{
        int i = (int)(arg);
        printf("sub pthread %d, id: %lu, pid:%d, tid:%ld\n", i, pthread_self(), getpid(), gettid());
       while (1);
}

int main(int argc, char* argv[])
{
        int i = 0;
        pthread_t pthreadid;
//        int ret = pthread_create(&pthreadid, NULL, fun1, NULL);
        printf("main pthread id:%lu, pid:%d, tid:%ld\n", pthread_self(), getpid(), gettid());
        
        for (; i < 6; i++){
                pthread_create(&pthreadid, NULL, fun1, (void *)i);
        }


        while (1);

        return 0;
}
