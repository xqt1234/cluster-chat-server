#pragma once
#include <string>
#include "TcpConnection.h"
#include "json.hpp"
#include "json_fwd.hpp"
#include "userdao.h"
#include "offlinemsgdao.h"
#include "groupdao.h"
#include "frienddao.h"
#include <functional>
#include "public.h"
#include <unordered_map>
#include <mutex>
#include "redisTool.h"
#include "tokenManager.h"
#include <unordered_set>
#include "baseservice.h"
#include "authService.h"
#include "friendService.h"
#include "groupService.h"
#include "sessionService.h"
#include "messageService.h"
using json = nlohmann::json;
using MsgHandle = std::function<void(const TcpConnectionPtr &conn, json &js,int userid)>;
class ChatService : public BaseService
{
public:
private:
    std::unordered_map<int, MsgHandle> m_handlemap;
    static const size_t MAX_JSON_LENGTH = 1024 * 1024; // 1MB
public:
    ChatService(/* args */);
    ~ChatService();
    MsgHandle getHandler(int msgid);
    ValidResult checkValid(std::string& str,json& data);
    void handMessage(const TcpConnectionPtr &conn,json& js);
    void removeConnection(const TcpConnectionPtr &conn);
private:
    AuthService m_authservcie;
    FriendService m_friendservice;
    GroupService m_groupservice;
    MessageService m_messageservice;
    SessionService m_sessionservice;
};