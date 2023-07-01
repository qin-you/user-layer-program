#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
void sys_error(const char* str)
{
        perror(str);
        exit(1);
}

int main()
{
        pid_t pid;
        int ret_pipe;
        int fd[2];

        ret_pipe = pipe(fd);
        if (ret_pipe == -1)
                sys_error("pipe error");
        pid = fork();
        if (pid == -1)
                sys_error("fork error");
        if (pid > 0)
        {
                close(fd[1]);
                dup2(fd[0],STDIN_FILENO);
                execlp("wc", "wc","-l",NULL);
                sys_error("execlp wc error");
        } else if (pid == 0)
        {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                execlp("ls","ls","-l",NULL);
                sys_error("execlp ls error");
        }
}
