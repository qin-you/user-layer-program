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

void capfun(int signum)
{
        if (signum == SIGINT)
                printf("captured %d signal\n", signum);
        if (signum == SIGBUS)
                printf("captured %d signal\n", signum);
}

int main(int argc, char *argv[])
{
//        signal(SIGINT, capfun);

        struct sigaction act, oldact;
        act.sa_handler = capfun;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;

        sigaction(SIGINT, &act, &oldact);
        sigaction(SIGBUS, &act, &oldact);
        while (1);
        return (0);
}
