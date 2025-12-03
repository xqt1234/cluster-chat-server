#include "clientService.h"
#include <iostream>
#include "json.hpp"
#include "public.h"
#include <functional>
#include "group.h"
#include <chrono>
#include <fstream>
#include "Logger.h"
ClientService::ClientService()
{
    using namespace std::placeholders;
    m_clientNet.connect();
    // 注册请求处理函数
    m_commandHandleMap.insert({"chat", std::bind(&ClientService::chatOne, this, _1)});
    m_commandHandleMap.insert({"addfriend", std::bind(&ClientService::addFriend, this, _1)});
    m_commandHandleMap.insert({"creategroup", std::bind(&ClientService::createGroup, this, _1)});
    m_commandHandleMap.insert({"joingroup", std::bind(&ClientService::joinGroup, this, _1)});
    m_commandHandleMap.insert({"groupchat", std::bind(&ClientService::chatGroup, this, _1)});

    // 注册响应处理函数
    m_ackHandlerMap.insert({static_cast<int>(MsgType::MSG_ADD_FRIEND_ACK),
                            std::bind(&ClientService::handle_MSG_ADD_FRIEND_ACK, this, _1)});
    m_ackHandlerMap.insert({static_cast<int>(MsgType::MSG_PRIVATE_CHAT_ACK),
                            std::bind(&ClientService::handle_MSG_PRIVATE_CHAT_ACK, this, _1)});
    m_ackHandlerMap.insert({static_cast<int>(MsgType::MSG_GROUP_CHAT_ACK),
                            std::bind(&ClientService::handle_MSG_GROUP_CHAT_ACK, this, _1)});
    m_ackHandlerMap.insert({static_cast<int>(MsgType::MSG_CREATE_GROUP_ACK),
                            std::bind(&ClientService::handle_MSG_CREATE_GROUP_ACK, this, _1)});
    m_ackHandlerMap.insert({static_cast<int>(MsgType::MSG_JOIN_GROUP_ACK),
                            std::bind(&ClientService::handle_MSG_JOIN_GROUP_ACK, this, _1)});
}
ClientService::~ClientService()
{
    m_stop = true;
    if (m_heartThread.joinable())
    {
        m_heartThread.join();
    }
}
std::string ClientService::getFriendName(int friendid)
{
    auto it = m_friendVec.find(friendid);
    if (it == m_friendVec.end())
    {
        std::cout << "消息来源id获取失败" << std::endl;
        return "";
    }
    std::string username = it->second.getUserName();
    return username;
}
bool ClientService::addFriend(std::string src)
{
    json js{
        {"friendid", atoi(src.c_str())}};
    json sendjs = buildRequest(js, MsgType::MSG_ADD_FRIEND);
    return m_clientNet.send(sendjs.dump());
}

bool ClientService::createGroup(std::string src)
{
    int index = src.find(":");
    if (index == -1)
    {
        std::cout << "群组名称输入错误" << std::endl;
        return false;
    }
    std::string groupname = src.substr(0, index);
    std::string groupdesc = src.substr(index + 1);
    json js{
        {"groupname", groupname},
        {"groupdesc", groupdesc}};
    json sendjs = buildRequest(js, MsgType::MSG_CREATE_GROUP);
    return m_clientNet.send(sendjs.dump());
}

bool ClientService::joinGroup(std::string src)
{
    int groupid = atoi(src.c_str());
    json js{
        {"groupid", groupid}};
    json sendjs = buildRequest(js, MsgType::MSG_JOIN_GROUP);
    return m_clientNet.send(sendjs.dump());
}

bool ClientService::chatOne(std::string src)
{
    int index = src.find(":");
    if (index == -1)
    {
        std::cout << "好友id输入错误" << std::endl;
        return false;
    }
    std::string friendstr = src.substr(0, index);
    std::string msg = src.substr(index + 1);
    json js{
        {"toid", atoi(friendstr.c_str())},
        {"userid", m_currentUser.getId()},
        {"msg", msg}};
    json sendjs = buildRequest(js, MsgType::MSG_PRIVATE_CHAT);
    return m_clientNet.send(sendjs.dump());
}

bool ClientService::chatGroup(std::string src)
{
    int index = src.find(":");
    if (index == -1)
    {
        std::cout << "群组编号输入错误" << std::endl;
        return false;
    }
    int groupid = atoi(src.substr(0, index).c_str());
    auto it = m_groupMap.find(groupid);
    if (it == m_groupMap.end())
    {
        std::cout << "你还不是该群组成员" << std::endl;
        return false;
    }
    std::string msg = src.substr(index + 1);
    json js{
        {"groupid", groupid},
        {"msg", msg}};
    json sendjs = buildRequest(js, MsgType::MSG_GROUP_CHAT);
    return m_clientNet.send(sendjs.dump());
}

int64_t ClientService::getCurrentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now.time_since_epoch())
        .count();
}

ClientService::ValidResult ClientService::sendlogin(std::string &str)
{
    if (m_clientNet.send(str))
    {
        std::string resmsg = m_clientNet.recvmsg();
        json jsres;
        ValidResult res = checkValid(resmsg, jsres);
        if (res.success)
        {
            setState(jsres["data"]);
        }
        return res;
    }
    return {false, ErrType::NETWORK_ERROR, "发送错误"};
}

bool ClientService::sendregister(std::string &str)
{
    if (m_clientNet.send(str))
    {
        std::string resmsg = m_clientNet.recvmsg();
        json jsres;
        ValidResult res = checkValid(resmsg, jsres);
        if (res.success)
        {
            jsres = jsres["data"];
            int userid = jsres["userid"];
            std::string username = jsres["username"];
            std::cout << "注册成功，账号是：" << userid << " 请记住您的账号" << std::endl;
            return true;
        }
        else
        {
            std::cout << res.message << std::endl;
        }
    }
    return false;
}

bool ClientService::handleService(std::string &str)
{

    auto it = m_commandHandleMap.find(str);
    if (it != m_commandHandleMap.end())
    {
        it->second(str);
        return true;
    }
    return false;
}

void ClientService::addCommand(std::string str, Func func)
{
    m_commandHandleMap.insert({str, func});
}

void ClientService::getRecv()
{
    std::string resstr = m_clientNet.recvmsg();
    json resjs;
    ValidResult res = checkValid(resstr, resjs);
    if (!res.success)
    {
        std::cout << res.message << std::endl;
        return;
    }
    int msgtype = resjs["msgid"];
    json datajs = resjs["data"];
    auto it = m_ackHandlerMap.find(msgtype);
    if (it != m_ackHandlerMap.end())
    {
        it->second(datajs);
    }
}

void ClientService::choiceUserToken(int userid)
{
    m_token = Token::getInstance().readToken(userid);
}

void ClientService::removeUserToken()
{
    Token::getInstance().clearToken(m_currentUser.getId());
}

std::unordered_map<std::string, Func> &ClientService::getHandleMap()
{
    return m_commandHandleMap;
}

void ClientService::stopService()
{
    m_stop = true;
}

void ClientService::startHeart()
{
    m_heartThread = std::thread(std::bind(&ClientService::sendHeart, this));
}

bool ClientService::setState(json &js)
{
    if (js.contains("userinfo") && js["userinfo"].is_object())
    {
        int userid = js["userinfo"].value("userid", -1);
        std::string username = js["userinfo"].value("username", "");
        if (userid == -1 || username == "")
        {
            return false;
        }
        m_currentUser.setId(userid);
        m_currentUser.setUserName(username);
    }
    if (js.contains("friends") && js["friends"].is_array())
    {
        for (auto &f : js["friends"])
        {
            if (!f.is_object())
            {
                continue;
            }
            User user;
            user.setId(f.value("userid", -1));
            user.setUserName(f.value("username", std::string()));
            user.setState(f.value("state", std::string()));
            m_friendVec.emplace(user.getId(), std::move(user));
        }
    }

    if (js.contains("groups") && js["groups"].is_array())
    {
        for (auto &g : js["groups"])
        {
            if (!g.is_object())
            {
                continue;
            }
            Group tgroup;
            int id = g.value("groupid", -1);
            tgroup.setId(id);
            tgroup.setDesc(g.value("groupdesc", std::string()));
            tgroup.setName(g.value("groupname", std::string()));
            m_groupMap.insert({tgroup.getId(), tgroup});
        }
    }
    if (js.contains("offlinemsg") && js["offlinemsg"].is_array())
    {
        for (auto &item : js["offlinemsg"])
        {
            if (!item.is_object())
            {
                continue;
            }
            int msgtype = item.value("msgid", -1);
            std::string res;
            if (msgtype == static_cast<int>(MsgType::MSG_PRIVATE_CHAT_ACK))
            {
                if (!item.contains("data"))
                {
                    continue;
                }
                json jsdata = item["data"];
                res = "好友" +jsdata.value("userid","") + "离线消息:" + jsdata.value("msg", std::string());
                m_offline_friend.push_back(res);
            }
            else if (msgtype == static_cast<int>(MsgType::MSG_GROUP_CHAT_ACK))
            {
                res = "群组离线消息:" + item.value("msg", std::string());
                m_offline_group.push_back(res);
            }
        }
    }
    if (js.contains("token"))
    {
        m_token = js["token"];
        Token::getInstance().saveToken(m_token, m_currentUser.getId());
    }
    return true;
}

json ClientService::buildRequest(json &obj, MsgType type)
{
    m_lastSendTime = getCurrentTimeMillis();
    json response = {
        {"msgid", static_cast<int>(type)},
        {"version", "1.0"},
        {"token", m_token},
        {"data", std::move(obj)}};
    return response;
}

void ClientService::sendHeart()
{
    while (!m_stop.load())
    {
        if (getCurrentTimeMillis() - m_lastSendTime > 5 * 1000)
        {
            json js;
            json sendjs = buildRequest(js, MsgType::MSG_HEARTBEAT);
            m_clientNet.send(sendjs.dump());
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void ClientService::setdisconnectionCallBack(const disconnectionCallBack &cb)
{
    m_clientNet.setDisconnectionCallBack(cb);
}

ClientService::ValidResult ClientService::checkValid(std::string &src, json &data)
{
    std::string errmsg;
    if (src.empty())
    {
        return {false, ErrType::MESSAGE_EMPTY, "收到的数据为空"};
    }
    if (src.length() > MAX_JSON_LENGTH)
    {
        return {false, ErrType::MESSAGE_TOO_LONG, "接收到的数据过长"};
    }
    try
    {
        data = json::parse(src);
    }
    catch (const std::exception &e)
    {
        return {false, ErrType::INVALID_REQUEST, "客户端json解析失败"};
    }
    if (data.value("msgid", -1) == -1)
    {
        errmsg = "消息类型错误";
        return {false, ErrType::PARAM_TYPE_ERROR, errmsg};
    }
    if (data["msgid"] == MsgType::MSG_ERROR)
    {
        if(data.contains("data") && data["code"] == ErrType::FRIEND_NOT_EXIST)
        {
            if(data.contains("message"))
            {
                errmsg = data["message"];
            }
        }
        return {false, ErrType::PARAM_TYPE_ERROR, errmsg};
    }
    return {true, ErrType::SUCCESS, ""};
}

void ClientService::handle_MSG_ADD_FRIEND_ACK(json &datajs)
{
    int frinedid = datajs.value("friendid", -1);
    std::string friendname = datajs.value("friendname", std::string());
    if (frinedid != -1)
    {
        User user;
        user.setId(frinedid);
        user.setUserName(friendname);
        m_friendVec.emplace(user.getId(), std::move(user));
    }
    std::cout << datajs.value("msg", std::string()) << std::endl;
    std::cout << "添加好友成功" << std::endl;
}
void ClientService::handle_MSG_PRIVATE_CHAT_ACK(json &datajs)
{
    std::string off_friend = datajs.value("msg", std::string());
    int fromid = datajs.value("fromid", -1);
    
    std::string username = getFriendName(fromid);
    m_offline_friend.push_back(username + "说:" + off_friend);
}
void ClientService::handle_MSG_GROUP_CHAT_ACK(json &datajs)
{
    std::string off_group = datajs.value("msg", std::string());
    std::string res = "群" + std::to_string(datajs.value("groupid", -1)) + "消息:" + off_group;
    m_offline_group.push_back(res);
}
void ClientService::handle_MSG_CREATE_GROUP_ACK(json &datajs)
{
    int groupid = datajs.value("groupid", -1);
    std::string groupname = datajs.value("groupname", "");
    std::string groupdesc = datajs.value("groupdesc", "");
    Group tgroup(groupname, groupdesc, groupid);
    m_groupMap.insert({groupid, tgroup});
    std::string off_group = datajs.value("msg", std::string());
    m_offline_friend.push_back(off_group);
}
void ClientService::handle_MSG_JOIN_GROUP_ACK(json &datajs)
{
    int groupid = datajs.value("groupid", -1);
    std::string groupname = datajs.value("groupname", "");
    std::string groupdesc = datajs.value("groupdesc", "");
    Group tgroup(groupname, groupdesc, groupid);
    m_groupMap.insert({groupid, tgroup});
    std::string off_group = datajs.value("msg", std::string());
    m_offline_friend.push_back(off_group);
}