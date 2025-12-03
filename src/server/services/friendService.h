#pragma once
#include "baseservice.h"
#include <functional>
#include "callBacks.h"
#include "frienddao.h"
class FriendService : public BaseService
{
private:
    GetConnCallBack m_getConn;
    FriendDAO m_frienddao;
public:
    FriendService(/* args */) = default;
    ~FriendService() = default;
    void addFriend(const TcpConnectionPtr &conn, json &js,int userid);
    void setGetConnCallBack(const GetConnCallBack& cb)
    {
        m_getConn = cb;
    }
    void isFriend(int userid,int friendid);
};

