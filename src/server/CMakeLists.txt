cmake_minimum_required(VERSION 3.10)
project(server)
aux_source_directory(. SRC_LIST_E)
aux_source_directory(./db SRC_LIST_2)
aux_source_directory(./model SRC_LIST_3)

include_directories(${PROJECT_SOURCE_DIR}/db)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRC_LIST_E} ${SRC_LIST_2} ${SRC_LIST_3})
target_link_libraries(${PROJECT_NAME} newmuduo mysqlclient)