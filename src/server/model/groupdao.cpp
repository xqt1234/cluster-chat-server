#include "groupdao.h"
#include "connectionPool.h"
#include "Logger.h"
// allgroup(id int unsigned primary key auto_increment,
// ownerid int unsigned not null,groupname varchar(50) not null,
// groupdesc varchar(200));
bool GroupDAO::createGroup(Group &tgroup, int userid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    std::string sql = "insert into allgroup(ownerid,groupname,groupdesc) values(?,?,?);";
    try
    {
        if (!conn->beginTransaction())
        {
            return false;
        }
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, userid);
        stmt->setString(2, tgroup.getName());
        stmt->setString(3, tgroup.getDesc());
        stmt->executeUpdate();
        // 获取自动生成的ID
        // 获取自增 ID
        auto stmt2 = conn->prepare("SELECT LAST_INSERT_ID()");
        auto res = stmt2->executeQuery();
        if (!res->next())
        {
            conn->rollback();
            return false;
        }
        int groupId = res->getInt(1);
        tgroup.setId(groupId); // 回写到对象中
        if (!addGroup(groupId, userid, "owner"))
        {
            conn->rollback();
            return false;
        }
        conn->commit();
        return true;
    }
    catch (const std::exception &e)
    {
        conn->rollback();
        LOG_ERROR("{}",e.what());
    }
    return false;
}

bool GroupDAO::removeGroup(int groupid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "delete from allgroup where groupid=?;";
        auto stmt = conn->prepare(sql);
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
    }
    return false;
}

bool GroupDAO::addGroup(int groupid, int userid, std::string role)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "insert into groupuser(groupid,userid,role) values(?,?,?);";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, groupid);
        stmt->setInt(2, userid);
        stmt->setString(3, role);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
    }
    return false;
}


Group GroupDAO::queryGroupByGroupId(int groupid)
{
    Group tgroup;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return tgroup;
    }
    try
    {
        std::string sql = "select id,groupname,groupdesc from allgroup where id=?;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, groupid);
        auto res = stmt->executeQuery();
        if (res->next())
        {
            tgroup.setId(res->getInt("id"));
            tgroup.setName(res->getString("groupname"));
            tgroup.setDesc(res->getString("groupdesc"));
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
    }
    return tgroup;
}

std::unordered_map<int,std::unordered_set<int>> GroupDAO::getAllGroupAndUsers()
{
    std::unordered_map<int,std::unordered_set<int>> resMap;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return resMap;
    }
    try
    {
        std::string sql = "SELECT groupid, userid FROM groupuser ORDER BY groupid;";
        auto stmt = conn->prepare(sql);
        auto res = stmt->executeQuery();
        while(res->next())
        {
            int tgroupid =  res->getInt("groupid");
            int userid = res->getInt("userid");
            resMap[tgroupid].insert(userid);
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
    }
    return resMap;
}

std::vector<Group> GroupDAO::queryGroupsByUserId(int userid)
{
    std::vector<Group> resvec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return resvec;
    }
    // create table groupuser(groupid int unsigned not null,
    // userid int unsigned not null,role enum('owner','admin','normal') default 'normal',
    // index idx_userid(userid),primary key(groupid,userid));
    try
    {
        std::string sql = "select a.id,a.groupname,a.groupdesc from allgroup a join groupuser b on a.id = b.groupid where b.userid=?;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, userid);
        auto res = stmt->executeQuery();
        while (res->next())
        {
            Group tgroup;
            tgroup.setId(res->getInt("id"));
            tgroup.setName(res->getString("groupname"));
            tgroup.setDesc(res->getString("groupdesc"));
            resvec.push_back(std::move(tgroup));
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
    }
    return resvec;
}
