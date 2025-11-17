#include "clientService.h"
#include <iostream>
#include "json.hpp"
#include "public.h"
#include <functional>
#include "group.h"
using json = nlohmann::json;
bool ClientService::addFriend(std::string src)
{
    
    json js;
    js["msgid"] = static_cast<int>(MsgType::MSG_ADD_FRIEND);
    //std::string friendstr = src.substr(0, index);
    js["friendid"] = atoi(src.c_str());
    js["userid"] = m_currentUser.getId();
    js["username"] = m_currentUser.getUserName();
    m_clientNet.send(js.dump());
    return true;
}
ClientService::ClientService()
{
    m_commandHandleMap.insert({"chat", std::bind(&ClientService::chatOne, this, std::placeholders::_1)});
    m_commandHandleMap.insert({"addfriend",std::bind(&ClientService::addFriend,this,std::placeholders::_1)});
}

Func ClientService::getChatService(std::string &command)
{
    return Func();
}

void ClientService::chatOne(std::string src)
{
    int index = src.find(":");
    if (index == -1)
    {
        std::cout << "好友id输入错误" << std::endl;
        return;
    }
    json js;
    js["msgid"] = static_cast<int>(MsgType::MSG_CHAT_ONE);
    std::string friendstr = src.substr(0, index);
    int friendid = atoi(friendstr.c_str());
    js["toid"] = atoi(friendstr.c_str());
    js["userid"] = m_currentUser.getId();
    js["username"] = m_currentUser.getUserName();
    js["msg"] = src.substr(index + 1);
    js["errno"] = 0;
    m_clientNet.send(js.dump());
}

ClientService &ClientService::getInstance()
{
    static ClientService service;
    return service;
}

bool ClientService::sendlogin(std::string &str)
{
    if (m_clientNet.send(str))
    {
        std::string resmsg = m_clientNet.recvmsg();
        return setState(resmsg);
    }
    return false;
}

bool ClientService::sendregister(std::string &str)
{
    if (m_clientNet.send(str))
    {
        std::string resmsg = m_clientNet.recvmsg();
        json js;
        try
        {
            js = json::parse(resmsg);
        }
        catch (const std::exception &e)
        {
            std::cerr << "json解析错误" << std::endl;
            return false;
        }
        if (js["errno"] != 0)
        {
            std::cout << "注册失败" << std::endl;
            return false;
        }
        else
        {
            int userid = js["userid"];
            std::string username = js["username"];
            std::cout << "注册成功，账号是：" << userid << " 请记住您的账号" << std::endl;
        }
        return true;
    }
    return false;
}

bool ClientService::handleService(std::string &str)
{

    auto it = m_commandHandleMap.find(str);
    if (it != m_commandHandleMap.end())
    {
        it->second(str);
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
    std::cout << resstr << std::endl;
    json js = json::parse(resstr);
    int msgtype = js["msgid"];
    if (js["errno"] != 0)
    {
        std::cout << js["errmsg"] << std::endl;
        return;
    }
    if (msgtype == static_cast<int>(MsgType::MSG_ADD_FRIEND))
    {
        int frinedid = js["friendid"];
        std::string friendname = js["friendname"];
        User user;
        user.setId(frinedid);
        user.setUserName(friendname);
        m_friendVec.push_back(user);
        std::cout << js["msg"] << std::endl;
        std::cout << "添加好友成功" << std::endl;
    }
    else if (msgtype == static_cast<int>(MsgType::MSG_CHAT_ONE))
    {
        std::string off_friend = js["msg"];
        std::string username = js["username"];
        m_offline_friend.push_back(username + "说:" + off_friend);
    }
    else if (msgtype == static_cast<int>(MsgType::MSG_CHAT_GROUP))
    {
        std::string off_group = js["msg"];
        m_offline_group.push_back(off_group);
    }
}

std::unordered_map<std::string, Func> &ClientService::getHandleMap()
{
    return m_commandHandleMap;
}

bool ClientService::setState(std::string &str)
{
    json js;
    try
    {
        js = json::parse(str);
    }
    catch (const std::exception &e)
    {
        std::cerr << "json解析错误" << std::endl;
        return false;
    }
    if (js["errno"] != 0)
    {
        std::cerr << js["errmsg"] << std::endl;
        return false;
    }
    m_currentUser.setId(js["userid"]);
    m_currentUser.setUserName(js["username"]);
    if (js.contains("friends"))
    {
        std::vector<std::string> friendsvec = js["friends"];
        for (auto &val : friendsvec)
        {
            json tmpjs = json::parse(val);
            User user;
            user.setId(tmpjs["userid"]);
            user.setUserName(tmpjs["username"]);
            user.setState(tmpjs["state"]);
            m_friendVec.emplace_back(std::move(user));
        }
    }
    if (js.contains("groups"))
    {
        std::vector<std::string> tmpgroupvec = js["groups"];
        for (auto &group : tmpgroupvec)
        {
            json tmpgroup = js.parse(group);
            Group tgroup;
            tgroup.setId(js["groupid"]);
            tgroup.setDesc(js["groupdesc"]);
            tgroup.setName(js["groupname"]);
            std::vector<std::string> tmpgroupuserstr = tmpgroup["groupusers"];
            for (auto &tuser : tmpgroupuserstr)
            {
                json tmpjs = json::parse(tuser);
                GroupUser groupuser;
                groupuser.setId(tmpjs["userid"]);
                groupuser.setUserName(tmpjs["username"]);
                groupuser.setState(tmpjs["state"]);
                groupuser.setRole(tmpjs["role"]);
                tgroup.getGroupUser().emplace_back(std::move(groupuser));
            }
        }
    }
    if (js.contains("offlinemsg"))
    {
        std::vector<std::string> vec = js["offlinemsg"];
        for (std::string &str : vec)
        {
            json offlinemsg = json::parse(str);
            int msgtype = offlinemsg["msgid"];
            std::string res;
            if (msgtype == static_cast<int>(MsgType::MSG_CHAT_ONE))
            {
                res += "好友离线消息";
                m_offline_friend.push_back(res);
            }
            else if (msgtype == static_cast<int>(MsgType::MSG_CHAT_GROUP))
            {
                res += "群组离线消息";
                m_offline_group.push_back(res);
            }
        }
    }
    return true;
}
