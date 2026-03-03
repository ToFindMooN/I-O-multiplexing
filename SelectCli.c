#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>
int Init_Socked()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        exit(1);
    }
    struct sockaddr_in saddr, caddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(res==-1)
    return -1;
    return sockfd;
}
int main()
{
 
    int sockfd = Init_Socked();
    if(-1==sockfd){
        return -1;
    }
    while(1)
    {
        printf("please InPut:");
        char buff[128] = {0};
        fgets(buff, 128, stdin);
        if (strncmp(buff, "bye", 3) == 0)
        {
            break;
        }
        int n = send(sockfd, buff, strlen(buff) - 1, 0);
        if (n <= 0)
        {
            printf("send Error");
            break;
        }
        memset(buff, 0, 128);
        n = recv(sockfd, buff, 127, 0);
        if (n <= 0)
        {
            printf("recv error");
            break;
        }
        printf("%s\n", buff);
    }
    close(sockfd);
    exit(0);
}
