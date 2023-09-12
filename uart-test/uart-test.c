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
#include <termios.h>
#include <getopt.h>

static struct uart_info {
        speed_t	    	speed;			// baud rate B115200 often
        unsigned char   dbit;                   // CS5 ~ CS8  often CS8  means 8bits for data
        char            parity;                 // N  O  E : not odd even
        unsigned char   sbit;                   // stop bit  1 or 2 bits
        char		devname[15];		// e.g.  /dev/ttyS1
} uart_info;

static char		io;
static int              fd;
static struct termios   old_cfg;


/* open, config */
static int uart_cfg(void)
{
	// int		fd;
	speed_t 	speed = B115200;
	/* new struct termios about uart config */
        struct termios 	term = {0};

	/* O_NOCTTY: not be terminal of this process */
	fd = open(uart_info.devname, O_RDWR|O_NOCTTY);
	if (fd < 0)
                return -1;

	/* save old config */
	if (tcgetattr(fd, &old_cfg)) {
                close(fd);
                return -1;
        }
        
	/* build new struct termios */
        // cfmakeraw(&term);			/* set term as raw mode */
        term.c_cflag |= CREAD;			/* set readable */
        if (cfsetspeed(&term, speed))		/* set baudrate*/
                return -1;
        term.c_cflag &= ~CSIZE;			/* clear setting of data size */
        term.c_cflag |= uart_info.dbit;		/* set data size */
	
	switch (uart_info.parity) {		/* set parity for new struct termios */
	case 'n':
	case 'N':
		term.c_cflag &= ~PARENB;		/* disable parity */
        	term.c_iflag &= ~INPCK;			/* disable input parity check */
		break;
	case 'o':
	case 'O':
		term.c_cflag &= (PARENB | PARODD);	/* parity enable and odd parity enable */
		term.c_iflag |= INPCK;			
		break;
	case 'e':
	case 'E':
		term.c_cflag &= (PARENB | ~PARODD);	/* ~PARODD denotes even parity */
		term.c_iflag |= INPCK;
	
	default:
		term.c_cflag &= ~PARENB;		/* we set no parity by default */
        	term.c_iflag &= ~INPCK;	
		break;
	}

	switch (uart_info.sbit) {			/* set stop bit for new struct termios */
	case 1:
		term.c_cflag &= ~CSTOP;			/* CSTOP:0 denotes 1 stop bit */
		break;
	case 2:
		term.c_cflag |= CSTOP;			/* CSTOP:1 denotes 2 stop bits */
		break;
	
	default:
		term.c_cflag &= ~CSTOP;
		break;
	}
        
        term.c_cc[VMIN]	 = 0;			/* minimal number of chars to read */
        term.c_cc[VTIME] = 0;			/* minimal wait time(ms) for a char */

        if (tcflush(fd, TCIOFLUSH) < 0)
                return -1;

        if (tcsetattr(fd, TCSANOW, &term) < 0)  /* set new struct termios work NOW */ 
                return -1;

        return 0;
}

static void read_handler(int sig)
{
        unsigned char buf[10] = {0};
        int     ret;
        int     n;

	ret = read(fd, buf, 1);
	for (n = 0; n < ret; n++)
                printf("0x%hhx ", buf[n]);
	printf("\n");

	return;


	// printf("read handler\n");
        // if (POLL_IN == info->si_code) {
        //         ret = read(fd, buf, 8);         // 8 bytes at most per time
        //         for (n = 0; n < ret; n++)
        //                 printf("0x%hhx ", buf[n]);
        //         printf("\n");
        // }
}

static void async_io_init(void)
{
	int                     flag;
        struct sigaction 	siga;
        
        flag = fcntl(fd, F_GETFL);		// get file flag
        fcntl(fd, F_SETFL, flag|FASYNC);	// enable async to the fd
        fcntl(fd, F_SETOWN, getpid());		// this process subscribe fd's signal

	signal(SIGIO, read_handler);		// begin to capture SIGIO signal
}

static void show_help(void)
{
        printf("code\n");
        exit(0);
}

static int _get_baudrate(int num)
{
	switch (num) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        default:
            return -1;
    }
}

static int _get_dbit(int num)
{
	switch (num) {
	case 5:
		return CS5;
	case 6:
		return CS6;
	case 7:
		return CS7;
	case 8:
		return CS8;
	default:
		return -1;
	}
}

int parse_args(int argc, char *argv[])
{
	int		ret     = 0;
	int             opt_idx;

        /* default config */
        uart_info.speed		= B115200;
        uart_info.dbit          = 8;
        uart_info.parity        = 'N';
        uart_info.sbit		= 1;
	strcpy(uart_info.devname, "/dev/ttyS2");
	io			= 'w';

	struct option 	long_options[] = {
                   {"speed",    required_argument, 0,  0 },
                   {"dbit",     required_argument, 0,  1 },
                   {"dev",      required_argument, 0,  2 },
                   {"parity",   required_argument, 0,  3 },     // N:No, O:Odd, E:Even
		   {"sbit",     required_argument, 0,  4 },
		   {"io",       required_argument, 0,  5 },     // rw
		   {"help",     required_argument, 0,  6 },
                   {0,          0,                 0,  0 }
        };

	while ((ret = getopt_long(argc, argv, "h", long_options, &opt_idx)) != -1) {
		switch (ret) {

                /* long option */
		case 0:
                        uart_info.speed = _get_baudrate(atoi(optarg));
                        break;
                case 1:
                        uart_info.dbit = _get_dbit(atoi(optarg));
                        break;
                case 2:
                        strcpy(uart_info.devname, optarg);
                        break;
                case 3:
                        uart_info.parity = atoi(optarg);
                        break;
                case 4:
                        uart_info.sbit = atoi(optarg);
                        break;
                case 5:
                        io = optarg[0];
                        break;
                case 6:
                        show_help();
                        break;

                /* char option */
                case 'h':
                        show_help();
                        break;
                default:
                        break;
		}
	}
}

int main(int argc, char *argv[])
{
	int		ret = 0;
	char		data[]={0xff, 0x77, 0x88, 0xff};
	char		tmp[8];

	parse_args(argc, argv);

        if (uart_cfg()) {
                tcsetattr(fd, TCSANOW, &old_cfg);
                close(fd);
                exit(1);
        }

        switch (io) {
        case 'r':
                async_io_init();
                while (1) {
                        sleep(1);
		}
                break;
        case 'w':
                while (1) {
                        write(fd, data, sizeof(data));
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
