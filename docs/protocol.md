### 协议示例与说明

#### 以下为登录请求和响应示例，稍后修改为token方式请求。

#### 请求
```json
{
    "msgid": 1000,
    "timestamp":1633024567890,
	"version": "1.0",
	"token":"",
    "data":{
		"userid":123,
        "password":"加密密码"
    }
}
```
#### 响应
```json
{
  "msgid": 1001,
  "code": 0,
  "message": "OK",
  "timestamp":1633024567900,
  "data": {
    "friends": [
      {"userid": 2, "username": "lisi", "state": "online"}
    ],
    "groups": []
  }
}
```
#### 说明：
1. msgid根据public.h里面定义的枚举来设定
2. 嵌套的时候使用对象形式，不得使用字符串化的json


