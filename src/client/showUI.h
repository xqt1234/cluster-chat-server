#pragma once
#include <string>
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include <vector>
#include <unordered_map>
#include <atomic>
#include "clientService.h"
class showUI
{
private:
    std::atomic<bool> m_quit{false};
    std::unordered_map<std::string, std::string> m_commandMap;
    bool m_hasLogin{false}; // 已经用token登录了就不跳转登录界面了
    ClientService m_service;
public:
    showUI(/* args */);
    ~showUI() = default;
    bool showLogin();   // 显示登录界面
    int showStart();    // 开始，先默认用token尝试登录。
    bool showRegister();
    void showUserMain();
    int showChatMain();
    void showofflineMsg();
    void showhelp();
    void quit();
    void logout();
private:
    int inputInt(const std::string &src, std::string inputerro = "输入错误，请重新输入");
    std::string inputString(const std::string &src);
    void showMsg();
    void disconnection();
};