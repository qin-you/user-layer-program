#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}
struct stu{
        char name[10];
        int score;
        int id;
};

int main(int argc, char *argv[])
{
        int fd = open("./tmp.txt", O_RDONLY);
        if (fd == -1)
                sys_err("open error");
        struct stu *p = mmap(NULL, sizeof(struct stu), PROT_READ, MAP_SHARED, fd, 0);
        if (p == MAP_FAILED)
                sys_err("mmap error");

        for (int i = 0; i < 100; i++){
                printf("%d, %s, %d\n", p->id, p->name, p->score);
                sleep(1);
        }

        close(fd);
        munmap(p, sizeof(struct stu));
                
}
