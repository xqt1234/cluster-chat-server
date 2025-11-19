#include "showUI.h"
#include <iostream>
#include <numeric>
#include "public.h"
#include "json.hpp"
#include "clientService.h"
#include <thread>
using json = nlohmann::json;
showUI::showUI()
{
    m_commandMap = {
        {"help", "显示所有支持的命令，格式:help"},
        {"addfriend", "添加好友，格式:addfriend:friendid"},
        {"chat", "一对一聊天，格式:chat:friendid:message"},
        {"creategroup", "创建群组，格式:creategroup:groupname:groupdesc"},
        {"addgroup", "加入群组，格式:addgroup:groupid"},
        {"groupchat", "群聊，格式:groupchat:groupid:message"},
        {"flush", "刷新界面,格式:flush"},
        {"quit", "退出登录,格式:quit"},
    };
    ClientService::getInstance().addCommand("help", std::bind(&showUI::showhelp, this));
    ClientService::getInstance().addCommand("quit", std::bind(&showUI::quit, this));
    ClientService::getInstance().addCommand("flush", std::bind(&showUI::showUserMain, this));
    ClientService::getInstance().setdisconnectionCallBack(std::bind(&showUI::disconnection,this));
}
bool showUI::showLogin()
{
    // 第一段
    int usercode = inputInt("请输入账号:");
    std::string password = inputString("请输入密码:");
    json js{
        {"userid", usercode}, 
        {"password", password}
    };
    json sendjs = ClientService::getInstance().buildResponse(js,MsgType::MSG_LOGIN);
    std::string res = sendjs.dump();
    std::cout << res << std::endl;
    bool ret = ClientService::getInstance().sendlogin(res);
    if(!ret)
    {
        std::cout << "连接服务器失败" << std::endl;
        return false;
    }
    return true;
}

int showUI::showStart()
{
    std::cout << "--------请输入------" << std::endl;

    std::cout << "1:登录" << std::endl;
    std::cout << "2:注册" << std::endl;
    std::cout << "0:退出";
    return inputInt("");
}

bool showUI::showRegister()
{
    std::string username = inputString("请输入用户名:");
    std::string password = inputString("请输入密码:");
    json js{
        {"username", username},
        {"password", password}
    };
    json sendjs = ClientService::getInstance().buildResponse(js,MsgType::MSG_REGISTER);
    std::string res = js.dump();
    return ClientService::getInstance().sendregister(res);
}

void showUI::showUserMain()
{
    const User &currentUser = ClientService::getInstance().getCurrentUser();
    if (currentUser.getId() == -1)
    {
        return;
    }
    const std::unordered_map<int,User> &friendVec = ClientService::getInstance().getFriend();
    std::cout << "当前登录用户编号：" << currentUser.getId() << " 用户名：" << currentUser.getUserName() << std::endl;
    std::cout << "好友列表：" << std::endl;
    for (auto &frienduser : friendVec)
    {
        std::cout << "用户编号：" << frienduser.second.getId() << " 用户名：" << frienduser.second.getUserName() << std::endl;
    }
    const std::vector<Group> &groupVec = ClientService::getInstance().getGroup();
    std::cout << "群组：" << std::endl;
    for (auto &groupusers : groupVec)
    {
        std::cout << "群号:" << groupusers.getId() << "群名："
                  << groupusers.getName() << "描述:"
                  << groupusers.getDesc() << std::endl;
    }
}

int showUI::showChatMain()
{
    showhelp();
    std::thread t1(std::bind(&showUI::showMsg, this));
    while (!m_quit)
    {
        std::string commandBuf = inputString("");
        if(m_quit)
        {
            break;
        }
        std::string command;
        int index = commandBuf.find(":");
        if (index == -1)
        {
            command = commandBuf;
        }
        else
        {
            command = commandBuf.substr(0, index);
        }
        auto &tmap = ClientService::getInstance().getHandleMap();
        auto it = tmap.find(command);
        if (it == tmap.end())
        {
            std::cout << "命令错误" << std::endl;
            continue;
        }
        std::string res = commandBuf.substr(index + 1, commandBuf.size() - index);
        it->second(res);
    }
    if(t1.joinable())
    {
        t1.join();
    }
    return 0;
}

void showUI::showhelp()
{
    for (auto &it : m_commandMap)
    {
        std::cout << it.second << std::endl;
    }
}
void showUI::quit()
{
    m_quit = true;
}

void showUI::showofflineMsg()
{
    const std::vector<std::string> &ofriendvec = ClientService::getInstance().getOfflineFriend();
    for (auto &str : ofriendvec)
    {
        std::cout << str << std::endl;
    }
    const std::vector<std::string> &ogroupvec = ClientService::getInstance().getOfflineGroup();
    for (auto &str : ogroupvec)
    {
        std::cout << str << std::endl;
    }
}

int showUI::inputInt(const std::string &src, std::string inputerro)
{
    int res;
    std::cout << src << std::endl;
    while (!(std::cin >> res))
    {
        std::cout << inputerro << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清空缓冲区
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清空缓冲区
    return res;
}

std::string showUI::inputString(const std::string &src)
{
    std::string res;
    std::cout << src << std::endl;
    std::getline(std::cin, res);
    return res;
}

void showUI::showMsg()
{
    ClientService &service = ClientService::getInstance();
    std::vector<std::string> &offline_friend = service.getOfflineFriend();
    std::vector<std::string> &offline_group = service.getOfflineGroup();
    while (!m_quit)
    {
        offline_friend.clear();
        offline_group.clear();
        service.getRecv();
        for (auto &str : offline_friend)
        {
            std::cout << str << std::endl;
        }
        for (auto &str : offline_group)
        {
            std::cout << str << std::endl;
        }
    }
}

void showUI::disconnection()
{
    std::cout << "服务器关闭了连接" << std::endl;
    m_quit = true;
}
