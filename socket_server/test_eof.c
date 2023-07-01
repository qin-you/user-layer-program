#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
        int fd = open("./test.txt", O_CREAT|O_TRUNC|O_RDWR, 0666);
        write(fd, "hello\nworld", 11);
        fsync(fd);
        lseek(fd, 0, SEEK_SET);
        char buf[100];
        int n = read(fd, buf, 11);
        if (n == -1) {
                perror("read err");
        }

        write(STDOUT_FILENO, buf, 11);

//        for (int i = 0; i < 122; i++) {
//                read(fd, &c, 1);
//                write(STDOUT_FILENO, &c, 1);
        close(fd);
}
