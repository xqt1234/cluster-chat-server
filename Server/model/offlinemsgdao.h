#pragma once
#include <vector>
#include <string>
class OffineMessageDAO
{
private:
    /* data */
public:
    OffineMessageDAO(/* args */) = default;
    ~OffineMessageDAO() = default;
    std::vector<std::string> query(int userid);
    bool remove(int userid);
    void insert(int userid,std::string msg);
};

