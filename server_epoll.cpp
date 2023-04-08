#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define MAX_EVENT_NUMBER 5
#define BUFFER_SIZE 10
int set_non_blocking( int fd ) {
    // fcntl(file control)对文件描述符的控制操作
    int old_state = fcntl(fd, F_GETFL); // F_GETFL获取fd的旧的状态标志
    int new_state = old_state | O_NONBLOCK; // 设置非阻塞标志
    fcntl( fd, F_SETFL, new_state); // F_SETFL 设置fd的状态标志
    return old_state; // 返回旧状态以便日后恢复
}

void addfd(int epollfd, int fd, bool enable_et) {
    epoll_event event;
    event.events = EPOLLIN ; // EPOLLIN数据可读事件 
    if(enable_et) {
        event.events |= EPOLLET;
    }
    event.data.fd = fd;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event);
    // 设置文件描述符为非阻塞
    set_non_blocking( fd );
}

void lt(epoll_event* events, int number, int epollfd, int listenfd ){
    char buf[BUFFER_SIZE];
    for(int i = 0; i < number; ++i) {
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd) {
            sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept( listenfd, (sockaddr*)&client_address, &client_addrlength);
            if(connfd != -1) {
                char remote[INET_ADDRSTRLEN];
                printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client_address.sin_addr, remote, INET_ADDRSTRLEN),
                        ntohs(client_address.sin_port));
            }
            addfd(epollfd, connfd, false);
        }
        else if(events[i].events & EPOLLIN) {
            printf("event trigger once\n");
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
            if(ret <= 0) {
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n", ret, buf);
            send(sockfd, buf, ret, 0);
        }
        else {
            printf("something else happened \n");
        }
    }
}

void et(epoll_event* events, int number, int epollfd, int listenfd) {
    char buf[BUFFER_SIZE];
    for ( int i = 0; i < number; ++i) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (sockaddr*)&client_address, &client_addrlength);
            if(connfd != -1) {
                char remote[INET_ADDRSTRLEN];
                printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client_address.sin_addr, remote, INET_ADDRSTRLEN),
                        ntohs(client_address.sin_port));
            }
            addfd( epollfd, connfd, true);
        }
        else if(events[i].events & EPOLLIN) {
            printf("event trigger once\n");
            while(1) {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
                if(ret < 0) {
                    if( (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                }
                else if (ret == 0) {
                    close(sockfd);
                }
                else {
                    printf("get %d bytes of content: %s\n", ret, buf);
                    send(sockfd, buf, ret, 0);
                }
            }
        }
        else {
            printf("something else happened \n");
        }
    }
}



int main(int argc, char* argv[]) {
    if (argc <= 2)
	{
		printf( "Usage: %s ip_address portname\n", argv[0] );
		return 0;
	}
    const char* ip = argv[1];
	int port = atoi( argv[2] );
    
    /*注册并设置监听socket*/
	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd >= 1 );
    struct sockaddr_in address;
	memset( &address, 0, sizeof( address ) );
	address.sin_family = AF_INET;
	address.sin_port = htons( port );
	inet_pton( AF_INET, ip, &address.sin_addr );
    int ret = 0;
	ret = bind( listenfd, (struct sockaddr*)( &address ), 
				sizeof( address ) );
	assert( ret != -1 );

    /*监听socket开始监听*/
	ret = listen( listenfd, 5 );
	assert( ret != -1 );


    /*创建就绪事件表event*/
    epoll_event events[ MAX_EVENT_NUMBER ];

    /*创建epoll内核事件表*/
    int epollfd = epoll_create( 5 );
    assert(epollfd != -1);
    // 向内核事件表添加监听socket
    addfd(epollfd, listenfd, true);

    while(1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0) {
            printf("epoll failure\n");
            break;
        }
        //lt(events, ret, epollfd, listenfd);
        et(events, ret, epollfd, listenfd); // 每个使用ET模式的文件描述符都应该是非阻塞的。如果是阻塞的，ET模式在处理事件时一直读/写势必会在最后一次阻塞
        
    }

    close(listenfd);
    
    return 0;

}