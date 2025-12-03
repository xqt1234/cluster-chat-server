#include "relationCache.h"
#include "groupdao.h"
RelationCache &RelationCache::getInstance()
{
    static RelationCache rcache;
    return rcache;
}

void RelationCache::addFriend(int uid, int fid)
{
    std::shared_lock<std::shared_mutex> lock(m_friendmtx);
    m_friendMap[uid].m_set.insert(fid);
    //m_friendMap[fid].m_set.insert(uid);
}

void RelationCache::initFriends(int uid, std::vector<int> &friends,uint64_t currentTime)
{
    std::lock_guard<std::shared_mutex> lock(m_friendmtx);
    m_friendMap[uid].m_set = std::unordered_set<int>(friends.begin(), friends.end());
    m_friendMap[uid].m_lastTime = currentTime;
}

bool RelationCache::isFriend(int uid, int fid)
{
    std::shared_lock<std::shared_mutex> lock(m_friendmtx);
    auto it = m_friendMap.find(uid);
    if(it != m_friendMap.end())
    {
        auto iit = it->second.m_set.find(fid);
        if(iit != it->second.m_set.end())
        {
            return true;
        }
    }
    return false;
}

void RelationCache::removeFriend(int uid, int fid)
{
    std::lock_guard<std::shared_mutex> lock(m_friendmtx);
    m_friendMap[uid].m_set.erase(fid);
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
