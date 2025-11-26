#pragma once
#include "baseservice.h"
class SessionService : BaseService
{
private:
    /* data */
public:
    SessionService(/* args */) = default;
    ~SessionService() = default;
    void removeConnection(const TcpConnectionPtr &conn);
};
