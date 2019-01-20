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
    int sockfd, cli_sd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int n;
    int ret;

    //创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket falied\n");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    
    //为服务器套接字绑定端口
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))< 0)
    {
        fprintf(stderr, "bind falied\n");
        exit(EXIT_FAILURE);
    }

    printf("Accepting connections... \n");
    
    //接收客户端的连接
    memset(buf, 0, MAXLINE);  // 清空数组
    cliaddr_len = sizeof(cliaddr);
    n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len);
    if (n == -1)
    {
        perror("recvfrom error");
    }
    fputs(buf, stdout);
    
    //为套接字指定目的地址，接下来的数据交互将可以采用recv()和send()
    if ((ret = connect(sockfd, (struct sockaddr* )&cliaddr, cliaddr_len)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
	    memset(buf, 0, MAXLINE);  // 清空数组
        cliaddr_len = sizeof(cliaddr);
        //n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len);
        n = recv(sockfd, buf, MAXLINE, 0);
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
