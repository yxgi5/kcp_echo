#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>

#define MAXLINE 1024
#define LOCAL_PORT    8001
#define SERV_PORT 8000
int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr,cliaddr;
    int sockfd, n;
    int ret;
    char buf[MAXLINE];
    //char str[INET_ADDRSTRLEN];
    socklen_t servaddr_len;

    //创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket falied\n");
        exit(EXIT_FAILURE);
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
    if ((ret = connect(sockfd, (struct sockaddr* )&servaddr, servaddr_len)) < 0)
    {
        fprintf(stderr, "connect falied\n");
        exit(EXIT_FAILURE);
    }
    
    //数据交互
    while(fgets(buf, MAXLINE, stdin) != NULL)
    {
        //n  = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr,servaddr_len);
        n = send(sockfd, buf, MAXLINE, 0);
        if (n == -1) //SOCKET_ERROR
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
    }
    close(sockfd);
    return 0;
}
