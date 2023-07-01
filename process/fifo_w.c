#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, char* argv[])
{
        int ret;
        int t = mkfifo("myfifo",0644);
        
        int fd = open("myfifo", O_WRONLY);
        for(int i=0;i<100;i++){
                
                write(fd,"1\n",1);
                sleep(1);
        }
        close(fd);
}
