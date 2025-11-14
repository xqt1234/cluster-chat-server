#pragma once
#include <string>
#include "TcpConnection.h"
#include "json.hpp"
using json = nlohmann::json;
class  ChatService
{
private:
    /* data */
public:
     ChatService(/* args */);
    ~ ChatService();
    void Login(const TcpConnectionPtr& conn,json& js);
};