#include "relationCache.h"
#include "groupdao.h"
RelationCache &RelationCache::getInstance()
{
    static RelationCache rcache;
    return rcache;
}

void RelationCache::addFriend(int uid, int fid)
{
    m_friendMap[uid].insert(fid);
    m_friendMap[fid].insert(uid);
}

void RelationCache::initFriends(int uid, std::vector<int> &friends)
{
    m_friendMap[uid] = std::unordered_set<int>(friends.begin(), friends.end());
}

bool RelationCache::isFriend(int uid, int fid)
{
    auto it = m_friendMap.find(uid);
    return it != m_friendMap.end() && it->second.count(fid) > 0;
}

void RelationCache::removeFriend(int uid, int fid)
{
    m_friendMap[uid].erase(fid);
    m_friendMap[fid].erase(uid);
}

void RelationCache::initAllGroupUsers()
{
    GroupDAO groupdao;
    m_groupMap = groupdao.getAllGroupAndUsers();
}

void RelationCache::addUserToGroup(int groupid, int userid)
{
    m_groupMap[groupid].insert(userid);
}

std::unordered_set<int> RelationCache::getAllUserFromGroup(int groupid)
{

    auto it = m_groupMap.find(groupid);
    if(it != m_groupMap.end())
    {
        return it->second;
    }
    return std::unordered_set<int>();
}
