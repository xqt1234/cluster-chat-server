#include "showUI.h"
#include <iostream>
#include <numeric>
#include "public.h"
#include "json.hpp"
using json = nlohmann::json;
std::string showUI::showLogin()
{
    // 第一段
    int usercode = inputInt("请输入账号:");
    std::string password = inputString("请输入密码:");
    json js;
    js["msgid"] = static_cast<int>(MsgType::MSG_LOGIN);
    js["userid"] = usercode;
    js["password"] = password;
    std::string res = js.dump();
    std::cout << res << std::endl;
    return res;
}

int showUI::showStart()
{
    std::cout << "--------请输入------" << std::endl;
    std::cout << "0:退出" << std::endl;
    std::cout << "1:登录" << std::endl;
    std::cout << "2:注册";
    return inputInt("");
}

void showUI::showUserMain(User &user,std::vector<User> &friendvec, std::vector<Group> &groupuvec)
{
    if(user.getId() == -1)
    {
        return;
    }
    std::cout << "当前登录用户编号：" << user.getId() << " 用户名：" << user.getUserName()  << std::endl;
    std::cout << "好友列表：" << std::endl;
    for(auto& frienduser: friendvec)
    {
        std::cout << "用户编号：" << frienduser.getId() << " 用户名：" << frienduser.getUserName() << std::endl;
    }
    std::cout << "群组：" << std::endl;
    for(auto& groupusers:groupuvec)
    {
        std::cout << "群号:" << groupusers.getId() << "群名："
        << groupusers.getName() << "描述:"
        << groupusers.getDesc() << std::endl;
        for(auto& groupuser: groupusers.getGroupUser())
        {
            std::string state = groupuser.getState() == "online" ? "在线" : "离线";
            std::cout << "用户编号：" << groupuser.getId() << " 用户名：" << groupuser.getUserName() << " " << state << std::endl;
        }
    }
}


int showUI::inputInt(const std::string &src)
{
    int res;
    std::cout << src << std::endl;
    while (!(std::cin >> res))
    {
        std::cout << "输入错误，请重新输入" << std::endl;
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
