#pragma once
#include "json.hpp"
#include "public.h"
using json = nlohmann::json;
struct ValidResult
{
    bool success;
    ErrType errType;
    std::string message;
};
class ResponseBuilder
{
public:
private:
    /* data */
public:
    ResponseBuilder(/* args */) = default;
    ~ResponseBuilder() = default;
    static json buildResponse(json &obj, MsgType type);
    static int64_t getCurrentTimeMillis();
    static json buildErrorResponse(ValidResult &&errmsg);
};
