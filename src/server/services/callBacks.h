#pragma once
#include <functional>
#include "TcpConnection.h"
#include "json.hpp"
#include "sessionService.h"
using json = nlohmann::json;
using GetConnCallBack = std::function<SessionService::ConnectInfo(int)>;
using KickCallBack = std::function<void(std::string)>;
using CheckCallBack = std::function<void(const TcpConnectionPtr &,json&, int)>;
