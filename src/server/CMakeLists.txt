cmake_minimum_required(VERSION 3.10)
project(server)
aux_source_directory(../common SRC_LIST_COMM)
aux_source_directory(. SRC_LIST_E)
aux_source_directory(./db SRC_LIST_2)
aux_source_directory(./model SRC_LIST_3)
aux_source_directory(./redis SRC_LIST_4)
aux_source_directory(./services SRC_LIST_5)
aux_source_directory(./public SRC_LIST_6)
add_executable(${PROJECT_NAME} 
${SRC_LIST_COMM} 
${SRC_LIST_E} 
${SRC_LIST_2} 
${SRC_LIST_3} 
${SRC_LIST_4} 
${SRC_LIST_5} 
${SRC_LIST_6}
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ./db
    ./public
    ./redis
    ./services
)
target_link_libraries(${PROJECT_NAME} minirpc nlohmann newmuduo mysqlcppconn redis++ hiredis ssl crypto pthread)