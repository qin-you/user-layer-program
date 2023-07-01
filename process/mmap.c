#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
        char *p;
        char *str = "hello";
        int fd = open("./hello.txt", O_CREAT|O_TRUNC|O_RDWR,0666);
        if (fd == -1)
                perror("open error");
        if (ftruncate(fd,10) == -1)
                perror("ftruncate error");
        int len = lseek(fd, 0, SEEK_END);
        printf("length:%d\n",len);

        p = mmap(NULL, 10, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
        if (p == MAP_FAILED)
                perror("mmap failed");
        
        close(fd);
        for (int i = 0; i < strlen(str) + 0; i++){
                *(p + i) = *(str + i);
        }
//        strcpy(p, str);       // write
        
        printf("%s\n",p);       //read

        munmap(p, 10);          //close shared mem zone
        return (0);
}
