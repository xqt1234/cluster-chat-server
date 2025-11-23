#pragma once
#include <string>
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include <vector>
#include <unordered_map>
#include <atomic>
#include "clientService.h"

/**
 * @brief 主要思想就是尽量把文字显示，帮助，操作等放在这里。注册登录前后，
 * 根据状态显示界面，供主函数调用。
 * 
 */
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
    // 开始，先默认用token后台尝试登录，登录失败再显示登录界面
    int showStart();
    // 显示登录界面
    bool showLogin();
    // 注册后显示用户注册到的账号
    bool showRegister();
    // 登录成功后，显示用户详情，好友，群组等
    void showUserMain();
    // 显示聊天页面，主要是接收用户输入的命令
    int showChatMain();
    // 显示离线消息。
    void showofflineMsg();
    // 显示帮助
    void showhelp();
    // 关闭程序
    void quit();
    // 退出登录
    void logout();
private:
    int inputInt(const std::string &src, std::string inputerro = "输入错误，请重新输入");
    std::string inputString(const std::string &src);
    void showMsg();
    void disconnection();
};