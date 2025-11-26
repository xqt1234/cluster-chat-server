#include "groupService.h"

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
