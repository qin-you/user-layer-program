#include "config.h"

ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++){
		if ( (rc = read(fd, &c, 1)) == 1){
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			*ptr = 0;
			return n - 1;
		} else
			return -1;
	}
	*ptr = 0;
	return n;
}

int main(int argc, char *argv[])
{
	int sockfd, n;
	struct sockaddr_in servaddr;
	
	if (argc != 2){
		perror("lack args");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket err");
		exit(1);
	}

	//bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr) == -1) {
		perror("inet_pton err");
		exit(1);
	}

	if ( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("connect err");
		exit(1);
	}

	char sendline[MAX_LINE], recvline[MAX_LINE];
	while ((n = read(STDIN_FILENO, sendline, MAX_LINE)) != 0) {
		write(STDOUT_FILENO, "read from stdin:", 16);
                write(STDOUT_FILENO, sendline, n);

		write(sockfd, sendline, n);
		
		if ((n = readline(sockfd, recvline, MAX_LINE))  == 0) {      //
			printf("server terminated prematurely\n");
			exit(1);
		}
                write(STDOUT_FILENO, "accept form server:", 19);
		write(STDOUT_FILENO, recvline, n);
	}
}
