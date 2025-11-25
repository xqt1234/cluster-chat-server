#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
class RelationCache
{
private:
    std::unordered_map<int, std::unordered_set<int>> m_friendMap;
    std::unordered_map<int, std::unordered_set<int>> m_groupMap;
public:
    static RelationCache& getInstance();
    void addFriend(int uid,int fid);
    void initFriends(int uid,std::vector<int>& friends);
    bool isFriend(int uid,int fid);
    void removeFriend(int uid, int fid); 
    void initAllGroupUsers();
    void addUserToGroup(int groupid,int userid);
    std::unordered_set<int> getAllUserFromGroup(int groupid);
private:
    RelationCache(/* args */) = default;
    ~RelationCache() = default;
};

