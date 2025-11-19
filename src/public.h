#pragma once

enum class MsgType
{
    // 认证模块 1000-1999
    MSG_LOGIN = 1000,
    MSG_LOGIN_ACK = 1001,
    MSG_LOGOUT = 1002,
    MSG_LOGOUT_ACK = 1003,

    MSG_REGISTER = 1100,
    MSG_REGISTER_ACK = 1101,

    // 好友管理 2000-2999
    MSG_ADD_FRIEND = 2000,
    MSG_ADD_FRIEND_ACK = 2001,
    MSG_DELETE_FRIEND = 2002,
    MSG_DELETE_FRIEND_ACK = 2003,
    MSG_GET_FRIEND_LIST = 2004,
    MSG_GET_FRIEND_LIST_ACK = 2005,

    // 群组管理 3000-3999
    MSG_CREATE_GROUP = 3000,
    MSG_CREATE_GROUP_ACK = 3001,
    MSG_JOIN_GROUP = 3100,
    MSG_JOIN_GROUP_ACK = 3101,
    MSG_LEAVE_GROUP = 3102,
    MSG_LEAVE_GROUP_ACK = 3103,

    // 消息通信 4000-4999
    MSG_PRIVATE_CHAT = 4000,
    MSG_PRIVATE_CHAT_ACK = 4001,
    MSG_GROUP_CHAT = 4100,
    MSG_GROUP_CHAT_ACK = 4101,

    // 系统消息 5000-5999
    MSG_OK = 5000,            // 默认消息
    MSG_ERROR = 5001,         // 错误消息
    MSG_HEARTBEAT = 5002,     // 心跳检测
    MSG_HEARTBEAT_ACK = 5003, // 心跳响应
};

enum class ErrType
{
    // ====== 成功 ======
    SUCCESS = 0,

    // ====== 系统级错误 1-99 ======
    SYSTEM_ERROR = 1,        // 系统错误
    DB_ERROR = 2,            // 数据库错误
    NETWORK_ERROR = 3,       // 网络错误
    SERVICE_UNAVAILABLE = 4, // 服务不可用

    // ====== 协议级错误 100-199 ======
    INVALID_REQUEST = 100,   // 请求格式错误
    MISSING_PARAM = 101,     // 缺少参数
    INVALID_PARAMS = 102,    // 参数错误
    PARAM_TYPE_ERROR = 103,  // 参数类型错误
    MSGID_NOT_SUPPORT = 104, // 不支持的msgid

    // ====== 认证模块 1000-1999 ======
    USER_NOT_EXIST = 1001,      // 用户不存在
    PASSWORD_ERROR = 1002,      // 密码错误
    USER_ALREADY_ONLINE = 1003, // 用户已在线
    NOT_LOGGED_IN = 1004,       // 未登录
    TOKEN_EXPIRED = 1005,       // Token过期
    USERNAME_EXISTS = 1006,     // 用户名已存在（注册时）
    REGISTER_FAILED = 1007,     // 注册失败

    // ====== 好友模块 2000-2999 ======
    FRIEND_ALREADY_EXISTS = 2001, // 好友已存在
    FRIEND_REQUEST_REPEAT = 2002, // 好友请求重复
    FRIEND_NOT_EXIST = 2003,      // 好友不存在

    // ====== 群组模块 3000-3999 ======
    GROUP_NOT_EXIST = 3001,        // 群组不存在
    NO_JOIN_PERMISSION = 3002,     // 无权加入群组
    ALREADY_IN_GROUP = 3003,       // 已在群组中
    NOT_GROUP_MEMBER = 3004,       // 非群组成员
    GROUP_OWNER_CANT_LEAVE = 3005, // 群主不能退群

    // ====== 消息模块 4000-4999 ======
    TARGET_OFFLINE = 4001,    // 对方不在线
    SEND_FAILED = 4002,       // 发送失败
    MESSAGE_EMPTY = 4003,     // 消息为空
    CHAT_TOO_FREQUENT = 4004, // 发送过于频繁
    MESSAGE_TOO_LONG = 4005,  // 消息过长
};