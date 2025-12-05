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
    FriendService();
    ~FriendService() = default;
    void addFriend(const TcpConnectionPtr &conn, json &js,int userid);
    void setGetConnCallBack(const GetConnCallBack& cb)
    {
        m_getConn = cb;
    }
    // void isFriend(int userid,int friendid);
private:
    void addFriendToLocal(std::unordered_map<std::string,std::string>& paramMap);
};

