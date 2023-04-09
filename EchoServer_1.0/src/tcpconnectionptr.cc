#include "tcpconnectionptr.h"

#include <sys/types.h>
#include <sys/socket.h>

#include "channel.h"

using namespace tiny_muduo;

TcpConnectionPtr::TcpConnectionPtr(EventLoop* loop, int connfd) 
    : loop_(loop),
      connfd_(connfd),
      channel_(new Channel(loop_, connfd_)) {
  channel_->SetReadCallback(std::bind(&TcpConnectionPtr::HandleMessage, this));
}

void TcpConnectionPtr::HandleMessage() {
  if (Recv() > 0) {
    message_callback_(this);
  }
}

void TcpConnectionPtr::Send(const string& message) {
  strcpy(buf_,message.c_str());
  send(connfd_, (const void *)(buf_), sizeof(buf_), 0);
}

std::string TcpConnectionPtr::Get() {
  string message(buf_);
  memset(buf_, '\0', sizeof(buf_));
  return message;
}

