#include "groupService.h"
using namespace sw::redis;
void GroupService::joinGroup(const TcpConnectionPtr &conn, json &js, int userid)
{
    int groupid = js.value("groupid", -1);
    if (groupid == -1)
    {
        json jsres = buildErrorResponse({true, ErrType::GROUP_NOT_EXIST, "群组不存在"});
        conn->send(jsres.dump());
        return;
    }
    bool res = m_groupdao.addGroup(groupid, userid, "normal");
    addToRedisGroup(groupid,userid);
    if (!res)
    {
        json jsres = buildErrorResponse({true, ErrType::DB_ERROR, "加入群组失败"});
        conn->send(jsres.dump());
        return;
    }
    Group group = m_groupdao.queryGroupByGroupId(groupid);
    
    json jsres{
        {"msg", "加入群组成功"},
        {"groupid", group.getId()},
        {"groupname", group.getName()},
        {"groupdesc", group.getDesc()}};
    json sendf = buildResponse(jsres, MsgType::MSG_JOIN_GROUP_ACK);
    conn->send(sendf.dump());
}

void GroupService::createGroup(const TcpConnectionPtr &conn, json &js, int userid)
{
    std::string groupname = js.value("groupname", "");
    std::string groupdesc = js.value("groupdesc", "");
    if (groupname == "" || groupdesc == "")
    {
        json jsres = buildErrorResponse({true, ErrType::INVALID_PARAMS, "群组参数错误"});
        conn->send(jsres.dump());
        return;
    }
    Group group(groupname, groupdesc);
    bool res = m_groupdao.createGroup(group, userid);
    if (!res)
    {
        json jsres = buildErrorResponse({true, ErrType::DB_ERROR, "创建群组失败"});
        conn->send(jsres.dump());
        return;
    }
    json jsres{
        {"msg", "创建群组成功"},
        {"groupid", group.getId()},
        {"groupname", group.getName()},
        {"groupdesc", group.getDesc()}};
    json sendf = buildResponse(jsres, MsgType::MSG_CREATE_GROUP_ACK);
    conn->send(sendf.dump());
}

void GroupService::queryGroup(int userid, json &js)
{
    std::vector<Group> groups = m_groupdao.queryGroupsByUserId(userid);
    if (!groups.empty())
    {
        std::vector<json> groupstr;
        groupstr.reserve(groups.size());
        for (auto &tgroup : groups)
        {
            groupstr.emplace_back(json{
                {"groupid", tgroup.getId()},
                {"groupname", tgroup.getName()},
                {"groupdesc", tgroup.getDesc()}});
        }
        js["groups"] = groupstr;
    }
}

void GroupService::initGroupInRedis()
{
    
    Redis& redis = m_redis.getRedis();
    std::unordered_map<int,std::unordered_set<int>> tmpmaps = m_groupdao.getAllGroupAndUsers();
    for(auto& map : tmpmaps)
    {
        std::string key = "groupset:" + std::to_string(map.first);
        std::unordered_set<int> tmpset = map.second;
        if(redis.exists(key) == 0)
        {
            redis.sadd(key,tmpset.begin(),tmpset.end());
        }
    }
}

std::vector<int> GroupService::getGroupUsers(int groupid)
{
    Redis& redis = m_redis.getRedis();
    std::string key = "groupset:" + std::to_string(groupid);
    std::vector<std::string> outvec;
    redis.smembers(key,std::back_inserter(outvec));
    std::vector<int> resvec;
    resvec.reserve(outvec.size());
    for(auto& str : outvec)
    {
        resvec.push_back(atoi(str.c_str()));
    }
    return resvec;
}

void GroupService::addToRedisGroup(int groupid, int userid)
{
    std::string key = "groupset:" + std::to_string(groupid);
    m_redis.getRedis().sadd(key,std::to_string(userid));
}
