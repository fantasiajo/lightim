cmake_minimum_required(VERSION 3.12)

project (im)

add_compile_options(-std=c++11)



add_executable(server server.cpp Acceptor.cpp Buffer.cpp EventLoop.cpp
 EventLoopThreadManager.cpp IOEventManager.cpp Socket.cpp TcpConnection.cpp
 TcpServer.cpp TcpSession.cpp ThreadManager.cpp Epoller.cpp util.cpp)

target_link_libraries(server -lpthread)
