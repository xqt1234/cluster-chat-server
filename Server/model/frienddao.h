#pragma once
#include <vector>
#include "user.h"
class FriendDAO
{
private:
    /* data */
public:
    FriendDAO(/* args */) = default;
    ~FriendDAO() = default;
    std::vector<User> query(int id);
    bool addFriend(int userid,int friendid);
};

