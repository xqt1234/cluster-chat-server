#pragma once
#include "CallBacks.h"
class EventLoop;
/**
 * @brief 主要思路就是封装一下文件描述符fd（socket函数创建，acceptor中accept获取），
 * 围绕该fd的所有操作，比如可读可写状态，事件，回调等等。
 * 在这里通过构造传入的loop，管理自身的状态。
 * 
 */
class Channel
{
private:
    int m_fd;
    int m_events{0};
    int m_revents{0};
    ReadEventCallBack m_readCallBack;
    EventCallBack m_writeCallBack;
    EventCallBack m_closeCallBack;
    EventCallBack m_errorCallBack;
    std::weak_ptr<void> m_tie;
    bool m_tied{false};
    int m_state{-1}; // 状态
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    EventLoop *m_loop;
    bool m_addedToLoop{false};
public:
    Channel(int fd,EventLoop* loop);
    ~Channel();
    void set_revents(int revt) { m_revents = revt; }
    int fd() { return m_fd; }
    void setState(int state) { m_state = state; }
    int getState() { return m_state; }
    int events() const { return m_events; }
    void handleEvent();
    void handleEventWithGuard();
    bool isNoneEvent() { return m_events == kNoneEvent; }
    bool isReadEvent() { return m_events & kReadEvent; }
    bool isWriteEvent() { return m_events & kWriteEvent; }
    void enableReading() { m_events |= kReadEvent; update();}
    void disableReading() { m_events &= ~kReadEvent; update();}
    void enableWriting() { m_events |= kWriteEvent; update();}
    void disableWriting() { m_events &= ~kWriteEvent; update();}
    bool isWriting() const { return m_events & kWriteEvent; }
    bool isReading() const { return m_events & kReadEvent; }
    void update();
    void remove();
    void disableAll(){m_events = kNoneEvent;update();}
    void setReadCallBack(ReadEventCallBack cb)
    {
        m_readCallBack = cb;
    }
    void setWriteCallBack(EventCallBack &cb)
    {
        m_writeCallBack = cb;
    }
    void setCloseCallBack(const EventCallBack &cb)
    {
        m_closeCallBack = cb;
    }
    void setErrorCallBack(EventCallBack &cb)
    {
        m_errorCallBack = cb;
    }
    void tie(const std::shared_ptr<void> &obj);
};
