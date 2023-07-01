#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

int main(int argc, char *argv[])
{
        pid_t pid = fork();
        if (pid > 0) {
                printf("I am parent");
                while(1){
                        printf("alive\n");
                        sleep(1);
                }
        } else if (pid == 0) {
                sleep(3);
                printf("I(child) will kill parent process %d\n", getppid());
                kill(getppid(), SIGKILL);
        }

        return 0;
}
