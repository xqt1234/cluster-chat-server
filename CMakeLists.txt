cmake_minimum_required(VERSION 3.10)
project(chatsever)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/include)
include_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_SOURCE_DIR}/ThirdParty/lib)
add_subdirectory(src/Server)
add_subdirectory(src/Client)