#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sys/epoll.h>
 
#define MAXFD 15
 
int sock_init();
 
int sock_init()
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd)
  {
    printf("ERROR IN sockfd()");
    exit(-1);
  }
  struct sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family = AF_INET;
  sadd.sin_port = htons(8080);
  sadd.sin_addr.s_addr = inet_addr("127.0.0.1");
  int n = bind(sockfd, &sadd, sizeof(sadd));
  if (-1 == n)
  {
    printf("Bind ERROR");
    return 0;
  }
  n = listen(sockfd, 5);
 
  return sockfd;
}
void eopll_add(int enfd, int fd)
{
  struct epoll_event ev;
  ev.data.fd = fd;
  ev.events = EPOLLIN;
  if (epoll_ctl(enfd, EPOLL_CTL_ADD, fd, &ev) == -1)
  {
    printf("ADD ERROR");
    return -1;
  }
}
void epoll_dlt(int enfd, int fd)
{
  if (epoll_ctl(enfd, EPOLL_CTL_DEL, fd, NULL) == -1)
  {
    printf("del err\n");
  }
}
void accept_client(int sockfd, int enfd)
{
  int n = accept(sockfd, NULL, NULL);
  if (n < 0)
  {
    printf("accept error");
    return;
  }
  printf("%d\n", n);
  epoll_add(enfd, n);
}
void recv_data(int enfd, int c)
{
  char buff[128] = {0};
  int n = recv(c, buff, 127, 0);
  if (n <= 0)
  {
    epoll_del(enfd, c);
    close(c);
    printf("client close(%d)\n", c);
    return;
  }
 
  printf("recv:%s\n", buff);
  send(c, "ok", 2, 0);
}
int main()
{
  int sockfd = sock_init();
  if (-1 == sockfd)
  {
    return -1;
  }
  int enfd = epoll_create(MAXFD);
  if (-1 == enfd)
  {
    return -1;
  }
  struct epoll_event evs[MAXFD];
  while (1)
  {
    int n = epoll_wait(enfd, evs, MAXFD, 5000);
    if (-1 == n)
    {
      printf("epoll_error\n");
    }
    else if (n == 0)
    {
      printf("time out\n");
    }
    else
    {
      for (int i = 0; i < n; i++)
      {
        if (evs[i].events & EPOLLIN)
        {
          if (evs[i].data.fd == sockfd)
          {
            accept_client(enfd, sockfd);
          }
          else
          {
            recv_data(enfd, evs[i].data.fd);
          }
        }
      }
    }
  }
}
