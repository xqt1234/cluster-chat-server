#include <iostream>
#include "redisTool.h"
#include <chrono>
// void showmessage(std::string str,std::string str2)
// {
//     std::cout << str <<" " << str2 <<std::endl;
// }
// int main()
// {
//     RedisTool tool;
//     tool.init_notify_handle(showmessage);
//     tool.connect();
//     //tool.subscribe(123);
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     tool.publish("kick_server02","hello world");
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     return 0;
// }
//g++ --std=c++20 test.cpp redisTool.cpp Logger.cpp config.cpp -lredis++ -lhiredis -lpthread

