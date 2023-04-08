#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        printf("Usage: %s ip_address portname\n", argv[0]);
        return 0;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    // 创建监听socket
    int listenfd = socket( PF_INET, SOCK_STREAM, 0);
    assert( listenfd >= 1 );
    // TCP/IP 协议族专用socket地址结构体 IPv4：sockaddr_in
    struct sockaddr_in address;
    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // htons: host to network short
    address.sin_port = htons(port);
    // 将用字符串表示的IP地址转换为用网络字节序整数表示的IP地址，并存在sin_addr中
    inet_pton( AF_INET, ip, &address.sin_addr);

    int ret = 0;
    
    // 将监听socket与socket地址绑定
    ret = bind(listenfd, (struct sockaddr*)(&address), sizeof(address));
    assert( ret != -1);
    // 创建一个监听队列以存放待处理的客户连接，backlog：处于全连接状态的socket上限
    ret = listen(listenfd, 5);
    assert( ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int sockfd = accept( listenfd, (struct sockaddr*)(&client), &client_addrlength);
    if(sockfd != -1) {
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN),
                ntohs(client.sin_port));
    }
    char buf_size[1024] = {0};
    int recv_size = 0;
    recv_size = recv( sockfd, buf_size, sizeof(buf_size), 0);

    int send_size = 0;
    send_size = send( sockfd, buf_size, recv_size, 0);

    close( sockfd );
    close( listenfd );
    return 0;
    
}