#pragma once
#include <string>
#include <vector>
#include "groupuser.h"

class Group
{
private:
    int m_groupid;
    std::string m_groupname;
    std::string m_groupdesc;
    std::vector<GroupUser> m_userVec;

public:
    Group(const std::string &groupname = "", const std::string &groupdesc = "",int id = -1)
        : m_groupid(id), m_groupname(groupname), m_groupdesc(groupdesc) {}
    ~Group() = default;
    void setId(int id) { m_groupid = id; }
    const int getId() const { return m_groupid; }
    void setName(const std::string &groupname) { m_groupname = groupname; }
    const std::string getName()const { return m_groupname; }
    void setDesc(const std::string &groupdesc) { m_groupdesc = groupdesc; }
    const std::string getDesc() const { return m_groupdesc; }
    const std::vector<GroupUser> &getGroupUser() const { return m_userVec; }
    std::vector<GroupUser> getGroupUser() { return m_userVec; }
};