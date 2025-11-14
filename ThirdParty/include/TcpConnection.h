#pragma once
#include <memory>
#include "InetAddress.h"
#include "CallBacks.h"
#include "Buffer.h"
class EventLoop;
class Channel;
class Socket;

/**
 * @brief 主要思路：围绕新连接的功能编写，比如发送数据，接收数据，对应设置这些回调函数
 * 数据缓存等。记录自身的状态，所属事件循环等以及连接建立和销毁接口。
 * 
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
    enum class StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
    /* data */
    int m_fd;
    InetAddress m_addr;
    std::unique_ptr<Channel> m_channel;
    EventLoop *m_loop;
    ConnectionCallBack m_ConnectionCallBack;
    Buffer m_inputBuffer;
    Buffer m_outBuffer;
    StateE m_state{StateE::kDisconnected};
    WriteCompleteCallBack m_WriteCompleteCallBack;
    // 发送的数据太大了来不及发送，通知机制。
    HighWaterCallBack m_HighWaterCallBack;
    size_t m_highWaterMark;
    MessageCallBack m_MessageCallBack;
    CloseCallBack m_CloseCallBack;
    uint64_t m_connId;
    std::unique_ptr<Socket> m_socket;
public:
    TcpConnection(EventLoop *loop, int cfd, InetAddress address);
    ~TcpConnection();
    EventLoop* getLoop(){return m_loop;}
    void connectEstablished();
    void connectDestroyed();
    void setConnId(uint64_t connid) { m_connId = connid; }
    uint64_t getConnId(){return m_connId;}
    bool isConnected(){return m_state == StateE::kConnected;}
    void setConnectionCallBack(ConnectionCallBack &cb)
    {
        m_ConnectionCallBack = cb;
    }
    void setCloseCallBack(const CloseCallBack &cb)
    {
        m_CloseCallBack = cb;
    }
    void setHighWaterCallBack(HighWaterCallBack &cb)
    {
        m_HighWaterCallBack = cb;
    }
    void setWriteCompleteCallBack(WriteCompleteCallBack &cb)
    {
        m_WriteCompleteCallBack = cb;
    }
    // 右值版本直接使用的时候std::move()得了
    void send(const std::string& str);
    void send(const char *data, int len);

    Buffer *inputBuffer()
    {
        return &m_inputBuffer;
    }

    Buffer *outputBuffer()
    {
        return &m_outBuffer;
    }
    void setMessageCallback(const MessageCallBack &cb)
    {
        m_MessageCallBack = cb;
    }
    void shutdown();

private:
    void sendInLoop(const void *data, size_t len);
    void shutdownInLoop();
    void handRead();
    void handleClose();
    void handleError();
};
