#pragma once
#include <string>
#include <vector>
class GroupUser;
class Group
{
private:
    int m_groupid;
    std::string m_groupname;
    std::string m_groupdesc;
    std::vector<GroupUser> m_userVec;
public:
    Group(int id = -1,const std::string& groupname =  "",const std::string& groupdesc = "")
        :m_groupid(id),m_groupname(groupname),m_groupdesc(groupdesc){}
    ~Group() = default;
    void setName(const std::string& groupname){m_groupname = groupname;}
    std::string getName(){return m_groupname;}
    void setDesc(const std::string& groupdesc){m_groupdesc = groupdesc;}
    std::string getDesc(){return m_groupdesc;}
    std::vector<GroupUser>& getGroupUser(){return m_userVec;}
};