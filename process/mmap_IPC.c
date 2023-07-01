#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

int main(int argc, char *argv[])
{
        int *p;
        pid_t pid;
        p = mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED)
                sys_err("mmap error");

        pid = fork();
        if (pid == -1)
                sys_err("fork err");
        if (pid == 0){
                *p = 1000;
        }
        if (pid > 0){
                sleep(1);
                printf("I am parent,I get %d\n",*p);
                munmap(p, 4);
        }
        return (4);
}
