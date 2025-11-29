#pragma once
#include "baseservice.h"
#include <functional>
#include "callBacks.h"
class FriendService : public BaseService
{
private:
    GetConnCallBack m_getConn;
public:
    FriendService(/* args */) = default;
    ~FriendService() = default;
    void addFriend(const TcpConnectionPtr &conn, json &js,int userid);
    void setGetConnCallBack(const GetConnCallBack& cb)
    {
        m_getConn = cb;
    }
};

