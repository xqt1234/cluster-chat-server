#pragma once
#include <string>
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include <vector>
#include <unordered_map>
#include <atomic>
class showUI
{
private:
    std::atomic<bool> m_quit{false};
    std::unordered_map<std::string, std::string> m_commandMap;
public:
    showUI(/* args */);
    ~showUI() = default;
    bool showLogin();
    int showStart();
    bool showRegister();
    void showUserMain();
    int showChatMain();
    void showofflineMsg();
    void showhelp();
    void quit();
private:
    int inputInt(const std::string &src, std::string inputerro = "输入错误，请重新输入");
    std::string inputString(const std::string &src);
    void showMsg();
    void disconnection();
};