#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "ikcp.h"

void millisecond_sleep(size_t n_millisecond)
{
    struct timespec sleepTime;
    struct timespec time_left_to_sleep;
    sleepTime.tv_sec = n_millisecond / 1000;
    sleepTime.tv_nsec = (n_millisecond % 1000) * 1000 * 1000;
    while( (sleepTime.tv_sec + sleepTime.tv_nsec) > 0 )
    {
        time_left_to_sleep.tv_sec = 0;
        time_left_to_sleep.tv_nsec = 0;
        int ret = nanosleep(&sleepTime, &time_left_to_sleep);
        if (ret < 0)
        {
            fprintf(stderr, "nanosleep error with err\n");
            //std::cerr << "nanosleep error with errno: " << errno << " " << strerror(errno) << std::endl;
        }
        sleepTime.tv_sec = time_left_to_sleep.tv_sec;
        sleepTime.tv_nsec = time_left_to_sleep.tv_nsec;
    }
}


/* get system time */
void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
}

/* get clock in millisecond 64 */
uint64_t iclock64(void)
{
    long s, u;
    uint64_t value;
    itimeofday(&s, &u);
    value = ((uint64_t)s) * 1000 + (u / 1000);
    return value;
}


uint32_t iclock()
{
    return (uint32_t)(iclock64() & 0xfffffffful);
}

#define MAXLINE 1024
#define LOCAL_PORT    8001
#define SERV_PORT 8000

int fd = 0;
struct sockaddr* dstAddr = NULL;
int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
	sendto(fd, buf, len, 0, dstAddr, sizeof(*dstAddr));
	return 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr,cliaddr;
    int sockfd, n;
    int ret;
    char buf[MAXLINE];
    //char str[INET_ADDRSTRLEN];
    socklen_t servaddr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    fd = sockfd;
    if (sockfd < 0)
    {
        fprintf(stderr, "socket falied\n");
        exit(EXIT_FAILURE);
    }
    
    
    // set socket non-blocking
    {
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags == -1)
        {
            //std::cerr << "get socket non-blocking: fcntl error return with errno: " << errno << " " << strerror(errno) << std::endl;
            fprintf(stderr, "get socket non-blocking err\n");
        }
        int ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        if (ret == -1)
        {
            //std::cerr << "set socket non-blocking: fcntl error return with errno: " << errno << " " << strerror(errno) << std::endl;
            fprintf(stderr, "set socket non-blocking err\n");
		}
    }

    
    //为套接字绑定本地地址信息
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(LOCAL_PORT);
    cliaddr.sin_addr.s_addr = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if ((ret = bind(sockfd, (struct sockaddr* )&cliaddr, addrlen)) < 0)
    {
        fprintf(stderr, "bind falied\n");
        exit(EXIT_FAILURE);
    }

    servaddr_len = sizeof(struct sockaddr_in);
    memset(&servaddr, 0, sizeof(servaddr));
    
    //为套接字指定目的地址信息
    servaddr.sin_family = AF_INET;
    //inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(SERV_PORT);
    dstAddr = &servaddr;
    
    ikcpcb *kcp1 = ikcp_create(0x11223344, (void*)0);
    kcp1->output = udp_output;
    

    while(fgets(buf, MAXLINE, stdin) != NULL)
    {

        //n  = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr,servaddr_len);
        n = ikcp_send(kcp1, buf, strlen(buf));
        if (n == -1)
        {
            perror("sendto error");
        }
        
        //n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, 0);
        //if (n == -1)
        //{
        //    perror("recvfrom error");
        //}
        
        //write(1,buf,n);  // 相当于printf
        //fputs(buf,stdout);
        memset(buf, 0, MAXLINE);
        while(1)
        {
            IUINT32 ts1 = iclock();
            ikcp_update(kcp1, ts1);
			n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &servaddr_len);
			if (n>0)
        	{
            	ikcp_input(kcp1, buf, n);
        	}

            int msgLen = ikcp_peeksize(kcp1);
		    if (msgLen > 0)
		    {
			    memset(buf, 0, MAXLINE);
			    if (msgLen > 0)
			    {
				    ikcp_recv(kcp1, buf, msgLen);
                    fputs(buf, stdout);
			    }
//			    msgLen = ikcp_peeksize(kcp1);
		    }
		    else
		    {
//		    	IUINT32 ts1 = iclock();
//				ikcp_update(kcp1, ts1);
		    	break;
		    }
        }
//        printf("hah\n");
//        IUINT32 ts1 = iclock();
//		ikcp_update(kcp1, ts1);
    }
    close(sockfd);
    return 0;
}
