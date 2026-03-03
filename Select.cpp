#include <stdio.h>
#include <stdlib.h>
#include<assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>
int Init_Socked()//初始化服务器端套接字
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);//使用IPV4的TCP链接
    if (-1 == sockfd)
    {
        exit(1);
    }
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;//地址族为IPV4
    saddr.sin_port = htons(9999);//端口号
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int n = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));//绑定套接字到指定的地址
    if (-1 == n)
    {
        printf("bind arror");
        exit(1);
    }
    n = listen(sockfd, 5);//监听套接字最大监听队列为5
    if (-1 == n)
        return -1;
    return sockfd;
}
 
void InitFds(int fds[], int n)//初始化记录服务器记录服务器套接字的数组
{
    int i = 0;
    for (; i < n; i++)
    {
        fds[i] = -1;
    }
}
 
void AddFdToFds(int fds[], int fd, int n)//将套接字描述符加入到数组中
{
    int i = 0;
    for (; i < n; i++)
    {
        if (fds[i] == -1)
        {
            fds[i] = fd;
            break;
        }
    }
}
void DelFdFromFds(int fds[], int fd, int n)//删除数组中的套接字描述符
{
    int i = 0;
    for (; i < n; i++)
    {
        if (fds[i] == fd)
        {
            fds[i] =-1;
            break;
        }
    }
}
int SetFdToset(fd_set *fd_set, int fds[], int n)
//将数组中的套接字描述符设置到fd_set变量中,并返回最大的文件描述符值
{
    FD_ZERO(fd_set);
    int i = 0, maxfd = fds[0];
    for (; i < n; i++)
    {
        if (fds[i] != -1)
        {
            FD_SET(fds[i], fd_set);
            if (fds[i] > maxfd)
            {
                maxfd = fds[i];
            }
        }
    }
    return maxfd;
}
//链接客户端和服务器
void GetClientLink(int sockfd, int fds[], int n)
{
    int c = accept(sockfd, NULL, NULL);
    if (c < 0)
    {
        return;
    }
    printf("SUCCESS TO LINK");
    AddFdToFds(fds, c, n);
}
//处理客户端的数据
void DealClientData(int fds[], int n, int clifd)
{
    char data[1024] = {0};
    int num = recv(clifd, data, 1024 - 1, 0);
    if (num <= 0)
    {
        DelFdFromFds(fds, clifd, n);
        close(clifd);
        printf("A Client disconnected");
    }
    else
    {
        printf("%d : %s \n", clifd, data);
        send(clifd, "OK", 2, 0);
    }
}
//处理数据函数
void DealReadyEvent(int fds[], int n, fd_set *fdset, int sockfd)
{
    int i = 0;
    for (; i < n; ++i)
    {
        if (fds[i] != -1 && FD_ISSET(fds[i], fdset))//当文件描述符存在并且在fdset集合中
        {
 
            if (fds[i] == sockfd)
            {
                GetClientLink(sockfd, fds, n);//就绪的是服务器端的套接字说明有新的客户端链接
            }
            else
            {
                DealClientData(fds, n, fds[i]);
            }
        }
    }
}
    int main()
    {
        int sockfd = Init_Socked();
        assert(sockfd!= -1);
        fd_set readfds;
        int fds[128];
        InitFds(fds,128);
        AddFdToFds(fds,sockfd,128);//将服务器端套接字加入到数组监听数组中
        while(1){
            int maxfd = SetFdToset(&readfds,fds,128);
            struct timeval timeout;
            timeout.tv_sec= 2;
            timeout.tv_usec=0;
            int n =select(maxfd+1,&readfds,NULL,NULL,&timeout);
            if(n<0){
                printf("select error");
                break;
            }
            else if(n==0)
            {
  printf("time out");
  continue;
 
           
}
DealReadyEvent(fds,128,&readfds,sockfd);
        }
        exit (0);
    }
