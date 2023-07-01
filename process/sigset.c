#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

void print_set(sigset_t set)
{
        printf("pending:");
        for (int j = 1; j < 33; j++){
                int tmp = sigismember(&set, j);
                if (tmp == 1)
                        printf("1");
                else if (tmp == 0)
                        printf("0");
                else
                        sys_err("sigismember err");
        }
        printf("\n");
}

int main(int argc, char* argv[])
{
        sigset_t set, oldset, pendingset;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        sigaddset(&set, SIGQUIT);
        sigaddset(&set, SIGBUS);
        // SIGKILL cannot be blocked
        sigaddset(&set, SIGKILL);
        int ret = sigprocmask(SIG_BLOCK, &set, &oldset);
        if (ret == -1)
                sys_err("sigprocmask err");

        while (1) {

                sigpending(&pendingset);
                print_set(pendingset);
                sleep(1);
                // printf("test\n");
        }
}
