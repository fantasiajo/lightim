cmake_minimum_required(VERSION 3.12)

project (im)

add_compile_options(-std=c++11)

add_executable(server server.cpp Acceptor.cpp Buffer.cpp EventLoop.cpp
 EventLoopThreadManager.cpp IOEventManager.cpp Socket.cpp TcpConnection.cpp
 TcpServer.cpp TcpSession.cpp ThreadManager.cpp Epoller.cpp util.cpp
 easylogging++.cc DB.cpp Msg.cpp UserManager.cpp)
target_link_libraries(server -lpthread -lmysqlclient)

add_executable(dbtest dbtest.cpp DB.cpp easylogging++.cc)
target_link_libraries(dbtest -lmysqlclient)

add_executable(signuptest signuptest.cpp Msg.cpp Socket.cpp easylogging++.cc md5.cpp)

add_executable(client client.cpp Acceptor.cpp Buffer.cpp EventLoop.cpp
 EventLoopThreadManager.cpp IOEventManager.cpp Socket.cpp TcpConnection.cpp
 TcpServer.cpp TcpSession.cpp ThreadManager.cpp Epoller.cpp util.cpp
 easylogging++.cc DB.cpp Msg.cpp UserManager.cpp md5.cpp)
target_link_libraries(client -lpthread -lmysqlclient)
