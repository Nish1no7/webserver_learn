cmake_minimum_required(VERSION 2.8.3)
project(echo_server)

add_compile_options(-std=c++11 )

include_directories(include)

add_executable(echo_server main.cpp)