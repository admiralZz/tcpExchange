project(tcpExchange)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)

add_subdirectory(server)
add_subdirectory(client)