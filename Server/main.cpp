#include <iostream>
#include "ChatServer.h"

EventLoop* gloop = nullptr;
void resethandler(int sigNum)
{
    gloop->quit();
    delete gloop;
    exit(0);
}
int main()
{
    gloop = new EventLoop();
    ChatServer m_server(gloop,9999,"192.168.65.4");
    m_server.start();
    gloop->loop();
    return 0;
}