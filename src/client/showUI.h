#pragma once
#include <string>
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include <vector>
class showUI
{
private:
    
public:
    showUI(/* args */) =default;
    ~showUI() = default;
    std::string showLogin();
    int showStart();
    void showUserMain(User& user,std::vector<User>& friendvec,std::vector<Group> &groupuvec);
    void showChatMain();
private:
    int inputInt(const std::string& src);
    std::string inputString(const std::string& src);

};