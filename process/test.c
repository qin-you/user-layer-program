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

int main()
{
        int i = 0;
        for (i = 0; i < 10; i++){
                pid_t pid = fork();
                if (pid == 0)
                        break;
                
        }
        if (i == 10)
                while (1);
        else
                return 0;
}

