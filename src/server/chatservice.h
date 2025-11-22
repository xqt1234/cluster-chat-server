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
using json = nlohmann::json;
using MsgHandle = std::function<void(const TcpConnectionPtr &conn, json &js,int userid)>;
class ChatService
{
public:
    struct ValidResult
    {
        bool success;
        ErrType errType;
        std::string message;
    };
private:
    UserDAO m_userdao;
    FriendDAO m_frienddao;
    GroupDAO m_groupdao;
    OffineMessageDAO m_offlinemsgdao;
    std::unordered_map<int, MsgHandle> m_handlemap;
    std::unordered_map<int, TcpConnectionPtr> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    std::mutex m_clientsmapMtx;
    RedisTool m_redis;
    std::unique_ptr<TokenManager> m_tokenManager;

    static const size_t MAX_JSON_LENGTH = 1024 * 1024; // 1MB
public:
    ChatService(/* args */);
    ~ChatService();
    void Login(const TcpConnectionPtr &conn, json &js,int userid);
    void LoginByToken(const TcpConnectionPtr &conn, json &js,int userid);
    void Register(const TcpConnectionPtr &conn, json &js,int userid);
    void ChatOne(const TcpConnectionPtr &conn, json &js,int userid);
    void addFriend(const TcpConnectionPtr &conn, json &js,int userid);
    void createGroup(const TcpConnectionPtr &conn, json &js,int userid);
    void addGroup(const TcpConnectionPtr &conn, json &js,int userid);
    MsgHandle getHandler(int msgid);
    ValidResult checkValid(std::string& str,json& data);
    inline long long getCurrentTimeMillis();
    json buildErrorResponse(ValidResult&& errmsg);
    int checkToken(std::string& str);
    void removeConnection(const TcpConnectionPtr &conn);
private:
    void queryGroup(int userid, json &js);
    json buildResponse(json& obj,MsgType type);
    void buildLoginInfo(const TcpConnectionPtr &conn, json &js,User& user,bool loginbytoken);
    void sendMsgToAllDevice(json &sendjson,int toid);
    
};