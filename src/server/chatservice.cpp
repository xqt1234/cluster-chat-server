#include "chatservice.h"
#include "Logger.h"
#include "relationCache.h"
#include <chrono>
#include "group.h"
#include "config.h"
#include "authService.h"
using namespace std::placeholders;
using json = nlohmann::json;
ChatService::ChatService()
{
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN), std::bind(&AuthService::login, &m_authservcie, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_REGISTER), std::bind(&AuthService::registerUser, &m_authservcie, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_PRIVATE_CHAT), std::bind(&MessageService::ChatOne, &m_messageservice, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_ADD_FRIEND), std::bind(&FriendService::addFriend, &m_friendservice, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_CREATE_GROUP), std::bind(&GroupService::createGroup, &m_groupservice, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_JOIN_GROUP), std::bind(&GroupService::joinGroup, &m_groupservice, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN_BY_TOKEN), std::bind(&AuthService::LoginByToken, &m_authservcie, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_GROUP_CHAT), std::bind(&MessageService::ChatGroup, &m_messageservice, _1, _2, _3)});
    
    m_kickchannelname = "kick:" + Config::getInstance().getValue("servername","server01");
    
}

ChatService::~ChatService()
{
}



MsgHandle ChatService::getHandler(int msgid)
{
    auto it = m_handlemap.find(msgid);
    if (it == m_handlemap.end())
    {
        auto func = [this](const TcpConnectionPtr &conn, json &js, int userid)
        {
            json jsres = buildErrorResponse({true, ErrType::PARAM_TYPE_ERROR, "参数类型错误"});
            conn->send(jsres.dump());
        };
        LOG_ERROR("参数类型错误{}", msgid);
        return func;
    }
    return it->second;
}


ChatService::ValidResult ChatService::checkValid(std::string &src, json &data)
{
    if (src.empty())
    {
        return {false, ErrType::MESSAGE_EMPTY, "请求数据为空"};
    }
    if (src.length() > MAX_JSON_LENGTH)
    {
        return {false, ErrType::MESSAGE_TOO_LONG, "请求数据过长"};
    }
    try
    {
        data = json::parse(src);
    }
    catch (const std::exception &e)
    {
        return {false, ErrType::INVALID_REQUEST, "json解析失败"};
    }

    if (data.is_null() || !data.is_object() || data.value("msgid", -1) == -1)
    {
        return {false, ErrType::PARAM_TYPE_ERROR, "消息类型错误"};
    }
    if (data["msgid"] == MsgType::MSG_LOGIN)
    {
        return {true, ErrType::SUCCESS, "登录，通过验证，不检验载荷"};
    }
    if (data["msgid"] != MsgType::MSG_LOGIN &&
        data["msgid"] != MsgType::MSG_REGISTER &&
        data.value("token", "") == "")
    {
        return {false, ErrType::TOKEN_EXPIRED, "没有token消息"};
    }
    if (!data.contains("data") || !data["data"].is_object())
    {
        return {false, ErrType::MESSAGE_EMPTY, "没有有效数据"};
    }
    return {true, ErrType::SUCCESS, ""};
}


void ChatService::handMessage(const TcpConnectionPtr &conn,json &js)
{
    int msgid = js["msgid"];
    std::string token = js["token"];
    int userid = m_authservcie.verifyToken(token);
    if (userid != -1 || (msgid == static_cast<int>(MsgType::MSG_LOGIN))
    || (msgid == static_cast<int>(MsgType::MSG_REGISTER)))
    {
        MsgHandle handle = getHandler(msgid);
        handle(conn, js["data"], userid);
    }
    else
    {
        json jsres = buildErrorResponse({true, ErrType::TOKEN_EXPIRED, "token过期"});
        conn->send(jsres.dump());
    }
}

void ChatService::removeConnection(const TcpConnectionPtr &conn)
{
    m_sessionservice.removeConnection(conn);
}
