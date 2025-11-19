#pragma once
#include "group.h"
class GroupDAO
{
private:
    /* data */
public:
    GroupDAO(/* args */) = default;
    ~GroupDAO() = default;
    bool insertGroup(Group& tgroup,int userid);
    bool removeGroup(int groupid);
    bool addGroup(int groupid,int userid,std::string role);
    std::vector<Group> queryGroup(int userid);
};

