#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int i = 0;
  int sockfd;
  struct ifconf ifconf;
  unsigned char buf[512];
  struct ifreq *ifreq;

  // 初始化ifconf
  ifconf.ifc_len = 512;
  ifconf.ifc_buf = buf;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  ioctl(sockfd, SIOCGIFCONF, &ifconf); // 获取所有接口信息

  // 接下来一个一个的获取IP地址
  ifreq = (struct ifreq *)buf;
  for (i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--) {
    printf("name = [%s]\n", ifreq->ifr_name);
    printf("local addr = [%s]\n",
           inet_ntoa(((struct sockaddr_in *)&(ifreq->ifr_addr))->sin_addr));
    ifreq++;
  }
  return 0;
}
