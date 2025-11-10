#pragma once
#include "EventLoopThread.h"
#include <vector>

/**
 * @brief 封装一下创建多个线程操作。轮询方式提供eventloop。
 * 主要还是提供给主线程acceptor新连接使用，所以，也没必要加锁之类的。
 * 
 */
class EventLoopThreadPoll
{
private:
    /* data */
    std::vector<EventLoopThread*> m_threads;
    int m_threadnum{0};
    int m_currentLoopNum{0};
    EventLoop* m_baseLoop;
public:
    EventLoopThreadPoll(EventLoop* baseLoop);
    ~EventLoopThreadPoll();
    void threadnum(int num);
    void start();
    EventLoop* getNextLoop();
};

