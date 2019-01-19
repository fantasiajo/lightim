cmake_minimum_required(VERSION 3.12)

project (im)

add_compile_options(-std=c++11)

add_executable(server server.cpp Buffer.cpp)
