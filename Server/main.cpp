#include <iostream>
#include "ChatServer.h"
#include <thread>
#include <vector>
#include "ConnectionPool.h"
EventLoop *gloop = nullptr;
void resethandler(int sigNum)
{
    gloop->quit();
    delete gloop;
    exit(0);
}
int main()
{
    ConnectionPool::getInstance().createDefaultTables("mysql.sql");
    return 0;
    gloop = new EventLoop();
    ChatServer m_server(gloop, 9999, "192.168.65.4");
    m_server.start();
    gloop->loop();
    std::cout << std::this_thread::get_id() << std::endl;
    return 0;
}
