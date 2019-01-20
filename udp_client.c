#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>

#define MAXLINE 1024

#define SERV_PORT 8000
int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    int sockfd, n;
    char buf[MAXLINE];
    //char str[INET_ADDRSTRLEN];
    socklen_t servaddr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket falied\n");
        exit(EXIT_FAILURE);
    }

    servaddr_len = sizeof(struct sockaddr_in);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    while(fgets(buf, MAXLINE, stdin) != NULL)
    {
        n  = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr,servaddr_len);
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
    }
    close(sockfd);
    return 0;
}
