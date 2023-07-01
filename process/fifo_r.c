#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
        int fd = open("myfifo",O_RDONLY);
        char buf[10];
        for(int i=0;i<100;i++){
                read(fd,buf,2);
                write(STDOUT_FILENO,buf,2);
        }
        close(fd);
}
