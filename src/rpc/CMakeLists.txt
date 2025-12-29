cmake_minimum_required(VERSION 3.10)
project(rpc_chat)
aux_source_directory(../common SRC_LIST_COMM)
aux_source_directory(. SRC_LIST_E)
aux_source_directory(./rpcservices SRC_LIST_2)
aux_source_directory(../server/model SRC_LIST_3)
aux_source_directory(../server/redis SRC_LIST_4)
aux_source_directory(../server/db SRC_LIST_5)
aux_source_directory(../server/public SRC_LIST_6)
# include_directories(${PROJECT_SOURCE_DIR}../db)
# include_directories(${PROJECT_SOURCE_DIR}/rpcservices)
# include_directories(${PROJECT_SOURCE_DIR}/redis)

# include_directories(${PROJECT_SOURCE_DIR}/services)
add_executable(${PROJECT_NAME} ${SRC_LIST_COMM} ${SRC_LIST_E} ${SRC_LIST_2} ${SRC_LIST_3} ${SRC_LIST_4} ${SRC_LIST_5} ${SRC_LIST_6})
target_include_directories(${PROJECT_NAME} PRIVATE
    ../server/db
    ../server/model
    ../server/public
    ./
    ./rpcservices
    ../server/public
)
target_link_libraries(${PROJECT_NAME} minirpc nlohmann newmuduo mysqlcppconn redis++ hiredis ssl crypto pthread)