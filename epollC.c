#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    assert(sockfd != -1); // 确认套接字创建成功
    
    struct sockaddr_in saddr; // 定义服务器的地址结构
    memset(&saddr, 0, sizeof(saddr)); // 将服务器地址结构清零
    saddr.sin_family = AF_INET; // 设置地址族为AF_INET
    saddr.sin_port = htons(6000); // 设置端口号为6000，并转换为网络字节序
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 设置IP地址为127.0.0.1
    
    int res = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)); // 连接到服务器
    assert(res != -1); // 确认连接成功
 
    while (1)
    {
        char buff[128] = {0}; // 用于存储用户输入的缓冲区
        printf("input:\n"); // 提示用户输入
        
        fgets(buff, 128, stdin); // 从标准输入获取用户输入
        if (strncmp(buff, "end", 3) == 0) // 如果用户输入"end"，则退出循环
        {
            break;
        }
        
        send(sockfd, buff, strlen(buff), 0); // 发送用户输入的数据到服务器
        
        memset(buff, 0, 128); // 清空缓冲区
        recv(sockfd, buff, 127, 0); // 接收服务器的响应
        printf("buff=%s\n", buff); // 打印服务器的响应
    }
    
    close(sockfd); // 关闭套接字
    exit(0); // 退出程序
}
