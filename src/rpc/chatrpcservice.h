#pragma once
#include <string>
#include <memory>
#include "provider.h"
#include "rpcService.h"
#include <vector>
class ChatRpcService
{
private:
    std::unique_ptr<miniRpc::ProVider> m_provider;
    std::vector<std::unique_ptr<miniRpc::RpcService>> m_serviceVec;
public:
    ChatRpcService();
    ~ChatRpcService() = default;
    void initServices();
};
