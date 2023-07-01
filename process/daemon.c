#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

int main(int argc, char *argv[])
{
        pid_t pid = fork();
        if (pid != 0)
                exit(0);

        setsid();
        chdir("/");
        umask(0777);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        
        while (1) {
                sleep(1);
        }
}
