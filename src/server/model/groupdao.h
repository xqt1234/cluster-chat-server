#pragma once
#include "group.h"
#include <unordered_map>
#include <unordered_set>
class GroupDAO
{
private:
    /* data */
public:
    GroupDAO(/* args */) = default;
    ~GroupDAO() = default;
    bool createGroup(Group& tgroup,int userid);
    bool removeGroup(int groupid);
    bool addGroup(int groupid,int userid,std::string role);
    std::vector<Group> queryGroupsByUserId(int userid);
    Group queryGroupByGroupId(int userid);
    std::unordered_map<int,std::unordered_set<int>> getAllGroupAndUsers();
};

