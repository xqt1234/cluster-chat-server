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
    char buf[1024];
    snprintf(buf, sizeof(buf),"insert into allgroup values(%d,'%s','%s');",userid,tgroup.getName().c_str(),tgroup.getDesc().c_str());
    return conn->update(buf);
}

bool GroupDAO::removeGroup(int groupid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf),"delet from allgroup where groupid=%d;",groupid);
    return conn->update(buf);
}

std::vector<Group> GroupDAO::queryGroup(int userid)
{
    std::vector<Group> res;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return res;;
    }
    //create table groupuser(groupid int unsigned not null,
    //userid int unsigned not null,role enum('owner','admin','normal') default 'normal',
    //index idx_userid(userid),primary key(groupid,userid));
    char buf[1024];
    snprintf(buf, sizeof(buf),"select a.id,a.groupname,a.groupdesc from allgroup a join groupuser b on a.id=b.groupid where b.userid=%d;",userid);
    MYSQL_RES* mres = conn->query(buf);
    if(mres != nullptr)
    {
        MYSQL_ROW row = nullptr;
        while((row = mysql_fetch_row(mres)) != nullptr)
        {
            Group tgroup;
            tgroup.setId(atoi(row[0]));
            tgroup.setName(row[1]);
            tgroup.setDesc(row[2]);
            res.push_back(std::move(tgroup));
        }
    }
    return res;
}
