#include "parseMessage.h"
#include "json.hpp"
#include "Logger.h"
#include "groupuser.h"
using json = nlohmann::json;
bool ParseMessage::parseLogin(std::string &src, User &user, std::vector<User>& friendvec,std::vector<Group>& groupvec)
{
    groupvec.clear();
    friendvec.clear();
    user.setId(-1);
    json js;
    try
    {
        js = json::parse(src);
    }
    catch (const std::exception &e)
    {
        std::cerr << "json解析错误" << std::endl;
        return false;
    }
    if (js["errno"] != 0)
    {
        std::cerr << "用户名或者密码错误" << std::endl;
        return false;
    }
    user.setId(js["userid"]);
    user.setUserName(js["username"]);
    if (js.contains("friends"))
    {
        std::vector<std::string> friendsvec = js["friends"];
        for (auto &val : friendsvec)
        {
            json tmpjs = json::parse(val);
            User user;
            user.setId(tmpjs["userid"]);
            user.setUserName(tmpjs["username"]);
            user.setState(tmpjs["state"]);
            friendvec.emplace_back(std::move(user));
        }
    }
    if (js.contains("groups"))
    {
        std::vector<std::string> tmpgroupvec = js["groups"];
        for (auto &group : tmpgroupvec)
        {
            json tmpgroup = js.parse(group);
            Group tgroup;
            tgroup.setId(js["groupid"]);
            tgroup.setDesc(js["groupdesc"]);
            tgroup.setName(js["groupname"]);
            std::vector<std::string> tmpgroupuserstr = tmpgroup["groupusers"];
            for (auto &tuser : tmpgroupuserstr)
            {
                json tmpjs = json::parse(tuser);
                GroupUser groupuser;
                groupuser.setId(tmpjs["userid"]);
                groupuser.setUserName(tmpjs["username"]);
                groupuser.setState(tmpjs["state"]);
                groupuser.setRole(tmpjs["role"]);
                tgroup.getGroupUser().emplace_back(std::move(groupuser));
            }
        }
    }
    return true;
}