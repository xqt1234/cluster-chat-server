cmake_minimum_required(VERSION 3.10)
project(server)
aux_source_directory(. SRC_LIST_E)

link_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRC_LIST_E})
target_link_libraries(${PROJECT_NAME} newmuduo)