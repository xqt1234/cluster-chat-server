#include "relationCache.h"

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
