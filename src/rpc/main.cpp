#include <iostream>
#include "config.h"
#include "chatrpcservice.h"
#include <condition_variable>
#include <mutex>
#include <csignal>
#include <atomic>
#include "rpcApplication.h"
std::condition_variable g_cv;
std::mutex g_mtx;
std::atomic<bool> shouldExit = false;

void signalHandle(int signal)
{
    shouldExit = true;
    g_cv.notify_one();
}
int main()
{
    std::signal(SIGINT,signalHandle);
    miniRpc::RpcApplication application;
    application.init();
    ChatRpcService service;
    service.initServices();
    std::unique_lock<std::mutex> lock(g_mtx);
    g_cv.wait(lock,[]{
        return shouldExit.load();
    });
    std::cout << "rpc程序退出..." << std::endl;
    return 0;
}