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
        struct stu stu1 = {"kangkang", 60, 1};
        int fd = open("./tmp.txt", O_CREAT|O_TRUNC|O_RDWR, 0644);
        if (fd == -1)
                sys_err("sys_err");
        if (ftruncate(fd, 20) == -1)
                sys_err("ftruncate err");
        char *p = mmap(NULL, sizeof(stu1), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (p == MAP_FAILED)
                sys_err("mmap err");
        
        for (int i = 0; i < 100; i++){
                stu1.id++;
                memcpy(p, &stu1, sizeof(stu1));
                sleep(1);
        }
        close(fd);
        munmap(p, sizeof(stu1));
}
