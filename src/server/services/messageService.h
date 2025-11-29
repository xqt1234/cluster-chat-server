#pragma once
#include "baseservice.h"
#include "callBacks.h"
class MessageService : public BaseService
{
public:
    using KickCallBack = std::function<void(std::string)>;
private:
    KickCallBack m_kickcallBack;
    GetConnCallBack m_getConn;
public:
    MessageService();
    ~MessageService() = default;
    void ChatOne(const TcpConnectionPtr &conn, json &js,int userid);
    void ChatGroup(const TcpConnectionPtr &conn, json &js,int userid);
    void handleRedisPublis(std::string, std::string);
    void setKickCallBack(const KickCallBack& cb)
    {
        m_kickcallBack = cb;
    }
    void setGetConnCallBack(const GetConnCallBack& cb)
    {
        m_getConn = cb;
    }
    
};
