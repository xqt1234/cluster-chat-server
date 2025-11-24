#include <iostream>
#include "chatserver.h"
#include <thread>
#include <vector>
#include "connectionPool.h"

#include "userdao.h"
#include "frienddao.h"
#include "group.h"
#include "groupuser.h"
#include "offlinemsgdao.h"
#include "user.h"
#include "Logger.h"
#include "DbChecker.h"
EventLoop *gloop = nullptr;
void resethandler(int sigNum)
{
    gloop->quit();
    delete gloop;
    exit(0);
}
int main()
{
    std::vector<std::string> sqlvec;
    sqlvec.push_back("user");
    sqlvec.push_back("friend");
    sqlvec.push_back("allgroup");
    sqlvec.push_back("groupuser");
    sqlvec.push_back("offlinemsg");
    DbChecker checker;
    bool res = checker.checkAll(sqlvec);
    if(!res)
    {
        std::cout << "数据库出错，终止程序" << std::endl;
    }
    // return 0;
    // User user;
    // user.setPassWord("xxxx");
    // user.setUserName("xiong");
    // UserDAO userdao;
    
    // if(userdao.inserUser(user))
    // {
    //     std::cout << "插入成功" << std::endl;
    // }else
    // {
    //     std::cout << "插入失败" << std::endl;
    // }
    // return 0;
    Logger::getInstance().setLogLevel(LogLevel::INFO);
    gloop = new EventLoop();
    ChatServer m_server(gloop, 9999, "192.168.65.4");
    m_server.start();
    gloop->loop();
    std::cout << std::this_thread::get_id() << std::endl;
    return 0;
}
