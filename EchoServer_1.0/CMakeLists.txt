cmake_minimum_required(VERSION 2.8.3)

project(echo_server_1.0)

add_compile_options( -std=c++11 )

include_directories(include)

add_executable(echo_server main.cc 
                           src/acceptor.cc 
                           src/channel.cc 
                           src/echo.cc 
                           src/epoller.cc
                           src/eventloop.cc
                           src/tcpconnectionptr.cc
                           src/tcpserver.cc)



