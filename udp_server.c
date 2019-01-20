#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#define MAXLINE 1024
#define SERV_PORT 8000
int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int sockfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Accepting connections... \n");
    while(1)
    {
	    memset(buf, 0, MAXLINE);  // 清空数组
        cliaddr_len = sizeof(cliaddr);
        n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len);
        if (n == -1)
        {
            perror("recvfrom error");
	}
        //buf[n] = '\0';
        //printf("%s", buf);
        fputs(buf, stdout);
        //printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, (socklen_t )sizeof(str)), ntohs(cliaddr.sin_port));
        //for (i = 0; i < n; i++)
        //    buf[i] = toupper(buf[i]);
        
        //n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        //if (n == -1)
        //{
        //    perror("sendto error");
        //}
    }
    return 0;
}
