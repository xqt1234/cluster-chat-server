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
#include "config.h"
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
    mymuduo::Logger::getInstance().setLogLevel(mymuduo::LogLevel::DEBUG);
    gloop = new EventLoop();
    Config& conf = Config::getInstance();
    std::string ipaddr = conf.getValue("serverip");
    uint16_t port = atoi(conf.getValue("serverport").c_str());
    ChatServer m_server(gloop, port, ipaddr);
    m_server.start();
    gloop->loop();
    std::cout << std::this_thread::get_id() << std::endl;
    return 0;
}
