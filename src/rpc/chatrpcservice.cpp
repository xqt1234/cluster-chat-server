#include "chatrpcservice.h"
#include "authRpcService.h"
#include <thread>
#include <chrono>
ChatRpcService::ChatRpcService()
    :m_provider(std::make_unique<miniRpc::ProVider>())
{
}

void ChatRpcService::initServices()
{
    std::shared_ptr<miniRpc::RpcService> service = std::make_shared<AuthRpcService>();
    service->setServiceName("UserService");
    m_provider->AddService(service);
}

// void ChatRpcService::start()
// {
//     m_provider->start();
//     std::this_thread::sleep_for(std::chrono::seconds(2));
// }
