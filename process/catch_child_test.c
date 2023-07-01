#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

void capfun(int signum)
{
        sleep(3);
        pid_t pid = wait(NULL);
        if (pid == -1)
                sys_err("wait error\n");
        printf("parent: wait success\n");

        while(wait(NULL) != -1)
                printf("wait +1\n");
                
}

int main(int argc, char *argv[])
{
        int i = 0;
        pid_t pid;
        struct sigaction act, oldact;

        
        for (; i < 10; i++) {
                pid = fork();
                if (pid == 0)
                        break;
        }
        if (i == 10) {
                act.sa_handler = capfun;
                sigemptyset(&(act.sa_mask));
                act.sa_flags = 0;
                sigaction(SIGCHLD, &act, &oldact);
        
                printf("im parent\n");
                while (1);
        } else {
                printf("child %d\n", i);
                return 0;
        }
}
