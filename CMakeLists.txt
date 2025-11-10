cmake_minimum_required(VERSION 3.10)
project(chatsever)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_subdirectory(Server)
add_subdirectory(Client)