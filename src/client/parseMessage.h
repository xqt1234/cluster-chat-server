#pragma once
#include <string>
#include <vector>
#include "user.h"
#include "group.h"
#include "groupuser.h"

class ParseMessage
{
private:
    /* data */
public:
    ParseMessage(/* args */) = default;
    ~ParseMessage() = default;
    bool parseLogin(std::string& src,User& user,std::vector<User>& friendvec,std::vector<Group>& groupvec);
};

