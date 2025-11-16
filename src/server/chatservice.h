#pragma once
#include <string>
#include "TcpConnection.h"
#include "json.hpp"
#include "userdao.h"
#include "offlinemsgdao.h"
#include "groupdao.h"
#include "frienddao.h"
#include <functional>
#include "public.h"
#include <unordered_map>
#include <mutex>
using json = nlohmann::json;
using MsgHandle = std::function<void(const TcpConnectionPtr& conn,json&js)>;
class  ChatService
{
private:
    UserDAO m_userdao;
    FriendDAO m_frienddao;
    GroupDAO m_groupdao;
    OffineMessageDAO m_offlinemsgdao;
    std::unordered_map<int,MsgHandle> m_handlemap;
    std::unordered_map<int,TcpConnectionPtr> m_clientsMap;
    std::mutex m_clientsmapMtx;
public:
     ChatService(/* args */);
    ~ ChatService();
    void Login(const TcpConnectionPtr& conn,json& js);
    void Register(const TcpConnectionPtr& conn,json& js);
    void ChatOne(const TcpConnectionPtr& conn,json& js);
    void addFriend(const TcpConnectionPtr& conn,json& js);
    MsgHandle getHandler(int msgid);
};