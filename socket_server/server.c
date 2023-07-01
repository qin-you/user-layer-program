#include "config.h"

int main(int argc, char *argv[])
{
        struct sockaddr_in servaddr, cliaddr;

        int listenfd, connfd;
        pid_t childpid;

        //char buf[MAX_LINE];

        socklen_t clilen;

        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket err");
                exit(1);
        }

        //bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
                perror("bind err");
                exit(1);
        }

        if (listen(listenfd, LISTENQ) == -1) {
                perror("listen err");
                exit(1);
        }

        while (1) {
                clilen = sizeof(cliaddr);
                if ((connfd = accept(listenfd, \
                (struct sockaddr *)&cliaddr, &clilen)) == -1) {
                        perror("accept err");
                        exit(1);
                }


                if ((childpid = fork()) == 0) {
                        close(listenfd);

                        ssize_t n;
                        char buff[MAX_LINE];
                        while ((n = read(connfd, buff, MAX_LINE)) > 0) {
                                write(STDOUT_FILENO, buff, n);
                                write(connfd, "Hi,client!", 11);
                                sleep(1);
                                write(connfd, buff, n);
                        }
                        exit(0);
                }
                close(connfd);
        }

        close(listenfd);
}
