#include "tcpserver.h"

#include "acceptor.h"
#include "tcpconnectionptr.h"

using namespace tiny_muduo;

TcpServer::TcpServer(EventLoop* loop, const Address& address)
    : loop_(loop),
      threads_(nullptr),
      acceptor_(new Acceptor(loop, address)) {
  acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::NewConnection, this, _1));
}

TcpServer::~TcpServer() {
  //delete threads_;
  delete acceptor_;
}

void TcpServer::NewConnection(int connfd) {  
  TcpConnectionPtr* ptr = new TcpConnectionPtr(loop_, connfd);
  ptr->SetConnectionCallback(connection_callback_);
  ptr->SetMessageCallback(message_callback_);
  loop_->RunOneFunc(std::bind(&TcpConnectionPtr::ConnectionEstablished, ptr));
}
