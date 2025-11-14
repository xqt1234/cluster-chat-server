#pragma once
#include "EventLoop.h"
#include <thread>
/**
 * @brief 主要思路：在这里对eventloop循环进行线程包装，构造后创建线程，
 * 在线程函数中创建eventloop并执行循环。
 */
class EventLoopThread
{
private:
    std::thread m_thread;
    EventLoop* m_loop{nullptr};
public:
    EventLoopThread(/* args */);
    ~EventLoopThread();
    EventLoop* getLoop();
private:
    void EventLoopFunc();

};

