/**************************
 * acquire  arp ip port rarp mac
***************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

void errlog(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int	sockfd;
	/* ETH_P_ALL: all ethernet packets in datalink layer */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
		errlog("fail to socket");

	unsigned char msg[1600] = {0};
	while (1) {
		if (read(sockfd, msg, sizeof(msg)) < 0)
		// if(recvfrom(sockfd, msg, sizeof(msg), 0, NULL, NULL) < 0)
			errlog("recvfrom failed");

		unsigned char dst_mac[18] = {0};
		unsigned char src_mac[18] = {0};
		unsigned short type;

		/* dst-6  src-6  type-2 */
		sprintf(dst_mac, "%02x:%02x:%02x:%02x:%02x:%02x", msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
		sprintf(src_mac, "%02x:%02x:%02x:%02x:%02x:%02x", msg[6], msg[7], msg[8], msg[9], msg[10], msg[11]);
		type = ntohs(*(unsigned short *)(msg + 12));
		printf("\nsource mac %s --> dst mac: %s\n", src_mac, dst_mac);

		switch(type) {
		case 0x0800: {
			/* ip datagram */
			unsigned char srcip[16] = {0};
			unsigned char dstip[16] = {0};
			unsigned char *ipheader = msg + 14;
			/* single byte has no byte order */
			unsigned char proto = *(unsigned char *)(ipheader + 9);

			sprintf(srcip, "%d.%d.%d.%d", ipheader[12], ipheader[13], ipheader[14], ipheader[15]);
			sprintf(dstip, "%d.%d.%d.%d", ipheader[16], ipheader[17], ipheader[18], ipheader[19]);
			printf("src ip:%s ---> dst ip %s\n", srcip, dstip);

			if (proto == 1)
				printf("icmp protocol\n");
			else if (proto == 6)
				printf("tcp protocol\n");
			else if (proto == 17)
				printf("udp protocol\n");

			unsigned char  *tcpheader = ipheader + 24;
			unsigned short srcport = ntohs(*(unsigned short *)tcpheader);
			unsigned short dstport = ntohs(*(unsigned short *)(tcpheader + 2));
			unsigned int seq = ntohl(*(unsigned int *)(tcpheader + 4));
			unsigned int ack = ntohl(*(unsigned int *)(tcpheader + 8));
			printf("src port %d --> dst port %d\n", srcport, dstport);
			break;
		}
		case 0x0806: {
			/* arp datagram in the 3rd layer*/
			printf("arp datagram\n");
			unsigned char srcip[16] = {0};
			unsigned char dstip[16] = {0};
			unsigned char *arpheader = msg + 14;

			sprintf(srcip, "%d.%d.%d.%d", arpheader[12], arpheader[13], arpheader[14], arpheader[15]);
			sprintf(dstip, "%d.%d.%d.%d", arpheader[16], arpheader[17], arpheader[18], arpheader[19]);
			printf("src ip:%s ---> dst ip %s\n", srcip, dstip);
			break;
		}
		case 0x0835:
			printf("rarp datagram\n");
			break;
		default:
			break;
		}
	}
}