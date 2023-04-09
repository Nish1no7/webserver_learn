#ifndef TINY_MUDUO_ADDRESS_H_
#define TINY_MUDUO_ADDRESS_H_

#include <string>
#include <cstring>

namespace tiny_muduo {

class Address {
 private:
  const char* ip_;
  const int port_;

 public:
  Address(const char* port = "2022") : ip_("127.0.0.1"), port_(atoi(port)) {}

  const char* ip() const { return ip_; }
  const int port() const { return port_; }
};

}
#endif

