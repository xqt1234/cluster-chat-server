#pragma once
#include "user.h"
class GroupUser : public User
{
private:
    std::string m_role;
public:
    GroupUser() = default;
    ~GroupUser() = default;
    void setRole(const std::string& role){m_role = role;}
    std::string getRole(){return m_role;}
};