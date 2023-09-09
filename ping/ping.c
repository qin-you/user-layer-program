/* Copyright (C) You Qin, 2023/09/09 Beijing */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <poll.h>


static int			sockfd;		// raw socket
static unsigned short		seq;		// icmp pkg's seq, grow 1 per time
static unsigned short		icmpid;		// often pid to distinguish other ping process 
static struct sockaddr_in	*dest;		// destination host, got with getaddrinfo()
static char			destip[16];	// destination host ip addr like 192.168.xxx.xxx

char				*datagram = "This is You Qin's datagram.";

struct {
	int			total;		// number of sent packages
	int			received;	// number of received pkg
	struct timeval		btv;		// begin time of this ping process
	struct timeval		etv;		// end time when press Ctrl-C
}statistics;


int create_socket(void)
{
	int			fd;
	int			size = 128 * 1024;
	struct protoent		*proto;
	proto = getprotobyname("icmp");
	if (!proto) {
		printf("getprotobyname failed, invalid protocol name\n");
		return -1;
	}
	fd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (fd < 0) {
		printf("create socket failed\n");
		return -1;
	}

	// optional: set recv buf
	if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))!=0){
		printf("setsockopt SO_RCVBUF error.\n\r");
		close(fd);
		return -1;
	}
	/* optional: set max wait time of recvfrom, (not necessary we use poll) */
	struct timeval timeout = {1,0};
	if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval))!=0){
		printf("setsockopt SO_RCVTIMEO error.\n\r");
		close(fd);
		return -1;
	}

	return fd;
}


/**********************************************
 * @param addr point to the whole package
 * @param len  length of the sending package
 * @return     chksum value
***********************************************/
unsigned short cal_chksum(unsigned short *addr,int len)
{       int nleft=len;
        int sum=0;
        unsigned short *w=addr;
        unsigned short answer=0;

        while(nleft>1)
        {
            sum+=*w++;
            nleft-=2;
        }

        if( nleft==1)
        {
            *(unsigned char *)(&answer)=*(unsigned char *)w;
            sum+=answer;
        }
        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return answer;
}


/**************************************************************
 * @description: create header and concatenate data with header
***************************************************************/
size_t icmp_pack(char *buf, int maxsz, char *data, int dsz)
{
	/* build header */
	struct icmp 	*icmphdr;
	icmphdr = (struct icmp *)buf;
	icmphdr->icmp_type = ICMP_ECHO;
	icmphdr->icmp_code = 0;
	icmphdr->icmp_cksum = 0;
	icmphdr->icmp_id = htons(icmpid);
	icmphdr->icmp_seq = htons(seq);

	seq = (seq + 1) % 0xffff;

	/* concatnate header with data part */
	char *pdata = buf + sizeof(struct icmp);
	memcpy(pdata, data, dsz);

	size_t	pkgsz = sizeof(struct icmp) + dsz;
	
	/* update checksum */
	icmphdr->icmp_cksum = cal_chksum((unsigned short *)buf, pkgsz);
	
	return pkgsz;
}


void *thread_send(void *t)
{
	int			pkgsize;
	char			sbuf[512];
	char			data[100];
	struct timeval		tv;
	
	while (1) {
		/* build package */
		gettimeofday(&tv, NULL);
		memcpy((void *)data, (void *)&tv, sizeof(tv));
		memcpy(data + sizeof(tv), datagram, sizeof(datagram));

		pkgsize = icmp_pack(sbuf, sizeof(sbuf), data, sizeof(data));

		if(sendto(sockfd, sbuf, pkgsize, 0, (struct sockaddr *)dest, sizeof(struct sockaddr)) < 0) {
			printf("sendto failed\n");
			continue;
		}

		statistics.total++;

		sleep(1);
	}

	return NULL;
}

int gettimeinterval(struct timeval *btv, struct timeval *etv)
{
	long	sec = etv->tv_sec - btv->tv_sec;
	long	us  = etv->tv_usec - btv->tv_usec;
	if (us < 0) {
		sec--;
		us += 1000000;
	}
	/* return ms */
	return (int)(sec*1000 + us/1000);
}

/*******************************************************
 * @description: 
 * - unpack package,include ip header, icmp header, data
 * - filter package with icmpid
 * - statistic 
********************************************************/
int icmp_unpack(char *rbuf, int size)
{
	struct ip	*iphdr;
	struct icmp	*icmphdr;

	if (size < sizeof(*iphdr) + sizeof(*icmphdr)) {
		printf("icmp_unpack: too small not target package\n");
		return -1;
	}

	iphdr = (struct ip *)rbuf;
	icmphdr = (struct icmp *)(rbuf + sizeof(struct ip));

	/* filter: type  id.  optional: check valid */
	if (icmphdr->icmp_type == ICMP_ECHOREPLY && ntohs(icmphdr->icmp_id) == icmpid) {
		statistics.received++;
		
		void *addr = &(iphdr->ip_src);
		char ip_buf[20];
		inet_ntop(dest->sin_family, addr, ip_buf, 20);

		char			*data;
		struct timeval		endtv;
		struct timeval		*begintvp;

		data = rbuf + sizeof(*iphdr) + sizeof(*icmphdr);
		begintvp = (struct timeval *)data;
		gettimeofday(&endtv, NULL);
		
		int	time_ms = gettimeinterval(begintvp, &endtv);
		
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%dms\n", size, ip_buf, ntohs(icmphdr->icmp_seq), iphdr->ip_ttl, time_ms);
	}
	return 0;
}

void *thread_recv(void *t)
{
	char		rbuf[512];
	int		size = 0;
	int		ret = 0;
	struct timeval	timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 200;

	struct pollfd	fds;
	fds.fd = sockfd;
	fds.events = POLLIN;

	while (1) {
		ret = poll(&fds, 1, 1);
		if (ret > 0) {
			size = recvfrom(sockfd, rbuf, sizeof(rbuf), 0, NULL, NULL);
			if (size < 0) {
				printf("recvfrom failed\n");
				continue;
			}
			icmp_unpack(rbuf, size);
		} else if (ret < 0) {
			printf("poll error\n");
		}
	}
	
	return NULL;
}


int init_vars(char *str)
{
	int			ret = 0;
	struct addrinfo 	hints;
	struct addrinfo 	*addrlist = NULL;

	/* init dest:  parse domain name and get sockaddr_in */
	memset(&hints, 0, sizeof(hints));			// IMPORTANT!!!
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	ret = getaddrinfo(str, NULL, &hints, &addrlist);
	if (ret != 0) {
		printf("wrong host \n");
		exit(1);
	}
	for (struct addrinfo *p = addrlist; p; p = p->ai_next) {
		/* first one could be ok, same ip address */
		dest = (struct sockaddr_in *)p->ai_addr;
		void *addr = &(dest->sin_addr);
		inet_ntop(dest->sin_family, addr, destip, 20);
		break;
	}

	/* init icmpid： cannot be icmp_id. it is a macro in ip_icmp.h*/
	icmpid = getpid() & 0xffff;

	seq = 1;

	return 0;
}

int ping_statistic(void)
{
	gettimeofday(&statistics.etv, NULL);
	
	int 	loss_rate = (int)((float)(statistics.total - statistics.received) / statistics.total * 100);
	int	timeinterval = gettimeinterval(&statistics.btv, &statistics.etv);

	printf("\n------ ping statistics ------\n");
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		statistics.total, statistics.received, loss_rate, timeinterval);
	
	return 0;
}

void signalhandler(int signum)
{
	exit(ping_statistic());
}

int main(int argc, char* argv[])
{
	int			ret = 0;
	pthread_t		send_tid = 0;
	pthread_t		recv_tid = 0;
	
	if (argc != 2) {
		printf("Usage: ./ping host\n");
		return -1;
	}

	init_vars(argv[1]);

	gettimeofday(&statistics.btv, NULL);

	sockfd = create_socket();
	if (sockfd < 0)
		return -1;

	int pkgsz = sizeof(struct icmp) + sizeof(struct timeval) + sizeof(datagram);
	printf("PING %s (%s) %x(%d) bytes of data.\n", argv[1], destip, pkgsz, pkgsz);

	ret = pthread_create(&send_tid, NULL, thread_send, NULL);
	if (ret < 0)
		return -1;

	ret = pthread_create(&recv_tid, NULL, thread_recv, NULL);
	if (ret < 0)
		return -1;

	signal(SIGINT, signalhandler);

	pthread_join(send_tid, NULL);
	pthread_join(recv_tid, NULL);

	return 0;
}