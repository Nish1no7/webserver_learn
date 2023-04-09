#include "echo.h"

#include "tcpserver.h"

EchoServer::EchoServer(tiny_muduo::EventLoop* loop, const tiny_muduo::Address& listen_addr)
    : loop_(loop),
      server_(loop, listen_addr) {
  server_.SetConnectionCallback(std::bind(&EchoServer::ConnectionCallback, this, _1));
  server_.SetMessageCallback(std::bind(&EchoServer::MessageCallback, this, _1));
}
