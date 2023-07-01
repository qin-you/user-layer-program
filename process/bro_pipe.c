#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <wait.h>
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
        int i=0;

        ret_pipe = pipe(fd);
        if (ret_pipe == -1)
                sys_error("pipe error");
        
        for(;i<2;i++){
                pid = fork();
                if (pid == -1)
                        sys_error("fork error");
                if (pid == 0)
                        break;
        }
        



        if (i == 2)
        {
                close(fd[0]);
                close(fd[1]);
                wait(NULL);
                wait(NULL);
        } else if (i == 0)
        {
                close(fd[0]);
                write(fd[1], "I probably am your big brother\n",31);
                close(fd[1]);
        }else if (i == 1)
        {       
                char* buf[31];
                close(fd[1]);
                read(fd[0], buf, 31);
                close(fd[0]);
                write(STDOUT_FILENO,"Re:",3);
                write(STDOUT_FILENO,buf,31);
        }
}
