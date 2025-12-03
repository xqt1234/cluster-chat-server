#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <shared_mutex>
class RelationCache
{
public:
    struct FriendRelation
    {
        // 缓存，避免每次登录都重新去查数据库,登录的时候，更新时间
        // 检查用户心跳的时候，可以每隔一小时，顺带检查一下是否需要从内存中移除。
        int64_t m_lastTime;
        std::unordered_set<int> m_set;
    };
    
private:
    std::unordered_map<int, FriendRelation> m_friendMap;
    std::shared_mutex m_friendmtx;
    std::unordered_map<int, std::unordered_set<int>> m_groupMap;
public:
    static RelationCache& getInstance();
    void addFriend(int uid,int fid);
    void initFriends(int uid,std::vector<int>& friends,uint64_t currentTime);
    bool isFriend(int uid,int fid);
    void removeFriend(int uid, int fid); 
    void initAllGroupUsers();
    void addUserToGroup(int groupid,int userid);
    std::unordered_set<int> getAllUserFromGroup(int groupid);
private:
    RelationCache(/* args */) = default;
    ~RelationCache() = default;
};

