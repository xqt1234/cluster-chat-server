#pragma once
#include <functional>
#include "TcpConnection.h"
#include "json.hpp"
#include "sessionService.h"
#include "baseservice.h"
using json = nlohmann::json;
using GetConnCallBack = std::function<BaseService::ConnectInfo(int)>;
using KickCallBack = std::function<void(std::string)>;
using CheckCallBack = std::function<void(const BaseService::ConnectInfo&)>;
