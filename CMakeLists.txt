cmake_minimum_required(VERSION 3.10)
project(chatsever)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/common)
include_directories(${PROJECT_SOURCE_DIR}/src/server)
include_directories(${PROJECT_SOURCE_DIR}/src/server/model)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_subdirectory(ThirdParty)
# include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/newmuduo/include)
# include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/nlohmann/include)
# link_directories(${PROJECT_SOURCE_DIR}/ThirdParty/newmuduo/lib)
add_subdirectory(src/server)
add_subdirectory(src/client)