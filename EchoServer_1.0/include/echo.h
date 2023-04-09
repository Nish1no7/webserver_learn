#ifndef TINY_MUDUO_ECHO_H_
#define TINY_MUDUO_ECHO_H_

#include <stdio.h>

#include <string>

#include "tcpserver.h"
#include "tcpconnectionptr.h"

namespace tiny_muduo {

  class Address;
  class EventLoop;

}

class EchoServer {
 public:
  EchoServer(tiny_muduo::EventLoop* loop, const tiny_muduo::Address& listen_addr);
  ~EchoServer() {}

  void Start() {
    server_.Start();
  }

  void ConnectionCallback(tiny_muduo::TcpConnectionPtr* connection_ptr) {
    printf("echo_server has a new connection \n");  
  }

  void MessageCallback(tiny_muduo::TcpConnectionPtr* connection_ptr) {
    std::string message(connection_ptr->Get());
    printf("echo_server get message \n");
    connection_ptr->Send(message);
  }

 private:
  tiny_muduo::EventLoop* loop_;
  tiny_muduo::TcpServer server_; 
};
#endif
