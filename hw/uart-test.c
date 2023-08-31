// #define _GNU_Source
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <signal.h>
#include <termios.h>

typedef struct uart_hw_cfg {
        unsigned int baudrate;
        unsigned char dbit;
        char parity;
        unsigned char sbit;
} uart_cfg_t;

static struct termios old_cfg;
static int fd;

static int uart_init(const char *device)
{
        fd = open(device, O_RDWR | O_NOCTTY);
        if (fd < 0)
                exit(1);

        if (tcgetattr(fd, &old_cfg)) {
                close(fd);
                exit(1);
        }

        return 0;
}

static int uart_cfg(const uart_cfg_t *cfg)
{
        struct termios term = {0};
        speed_t speed;

        cfmakeraw(&term);
        term.c_cflag |= CREAD;  // readable

        speed = B115200;

        if (cfsetspeed(&term, speed))
                return -1;

        term.c_cflag &= ~CSIZE;
        term.c_cflag |= CS8;    // data bits

        term.c_cflag &= ~PARENB;        // clear parity enable
        term.c_iflag &= ~INPCK;         // clear input parity check

        term.c_cflag &= ~CSTOPB;        // one stop bit

        term.c_cc[VMIN] = 0;
        term.c_cc[VTIME] = 0;

        if (tcflush(fd, TCIOFLUSH) < 0)     // flush
                return -1;

        if (tcsetattr(fd, TCSANOW, &term) < 0)  // make configs work
                return -1;

        return 0;
}

static void io_handler(int sig, siginfo_t *info, void *context)
{
        unsigned char buf[10] = {0};
        int     ret;
        int     n;

        if (POLL_IN == info->si_code) {
                ret = read(fd, buf, 8);         // 8 bytes at most per time
                for (n = 0; n < ret; n++)
                        printf("0x%hhx ", buf[n]);
                printf("\n");
        }
}

static void async_io_init(void)
{
        struct sigaction siga;
        int                     flag;

        flag = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, flag|FASYNC);
        fcntl(fd, F_SETOWN, getpid());

        // replace SIGIO with SIGRTMIN to add priority
        // fcntl(fd, F_SETSIG, SIGRTMIN);

        // indicate sig handler
        siga.sa_sigaction = io_handler;
        // set allowing complicated handler. simple default
        siga.sa_flags = SA_SIGINFO;
        // clear mask
        sigemptyset(&siga.sa_mask);
        // bind handler
        sigaction(SIGRTMIN, &siga, NULL);
}

int main(int argc, char *argv[])
{
        uart_cfg_t cfg;
        char *device = NULL;
        int rw_flag = -1;
        unsigned char w_buf[10] = {0x77,0x77,0x77,0x77};
        int n;

        // parse args
        rw_flag = 1;
        cfg.baudrate = 115200;
        cfg.dbit = 8;
        cfg.parity = 'N';
        cfg.sbit = 1;
        device = "/dev/ttyS0";

        if (uart_init(device))
                exit(1);

        if (uart_cfg(&cfg)) {
                tcsetattr(fd, TCSANOW, &old_cfg);
                close(fd);
                exit(1);
        }

        switch (rw_flag) {
        case 0: //read
                async_io_init();
                while (1)
                        sleep(1);
                break;
        case 1:  // write
                while (1) {
                        write(fd, w_buf, 4);
                        sleep(1);
                }
                break;
        default:
                break;
        }

        tcsetattr(fd, TCSANOW, &old_cfg);
        close(fd);
        exit(0);

}























