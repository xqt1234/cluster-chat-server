#include "groupdao.h"
#include "connectionPool.h"
//allgroup(id int unsigned primary key auto_increment,
//ownerid int unsigned not null,groupname varchar(50) not null,
//groupdesc varchar(200));
bool GroupDAO::insertGroup(Group &tgroup, int userid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    std::string sql = "insert into allgroup(ownerid,groupname,groupdesc) values(?,?,?);";
    try
    {
        auto stmt = conn->prepare(sql);
        stmt->setInt(1,userid);
        stmt->setString(2,tgroup.getName());
        stmt->setString(3,tgroup.getDesc());
        stmt->executeUpdate();
        // 获取自动生成的ID
        // 获取自增 ID
        auto stmt2 = conn->prepare("SELECT LAST_INSERT_ID()");
        std::unique_ptr<sql::ResultSet> res(stmt2->executeQuery());
        if (res->next()) {
            int groupId = res->getInt(1);
            tgroup.setId(groupId);   // 回写到对象中
        }
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
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
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        
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
        stmt->setInt(1,groupid);
        stmt->setInt(2,userid);
        stmt->setString(3,role);
        stmt->execute();
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        
    }
    return false;
}

std::vector<Group> GroupDAO::queryGroup(int userid)
{
    std::vector<Group> resvec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return resvec;
    }
    //create table groupuser(groupid int unsigned not null,
    //userid int unsigned not null,role enum('owner','admin','normal') default 'normal',
    //index idx_userid(userid),primary key(groupid,userid));
    try
    {
        std::string sql = "select id,groupname,groupdesc from allgroup where ownerid=?;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1,userid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        while(res->next())
        {
            Group tgroup;
            tgroup.setId(res->getInt("id"));
            tgroup.setName(res->getString("groupname"));
            tgroup.setDesc(res->getString("groupdesc"));
            resvec.push_back(std::move(tgroup));
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return resvec;
}
