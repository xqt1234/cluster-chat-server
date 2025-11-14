#include <iostream>
#include "clientNet.h"
#include <string>
#include "json.hpp"
#include "public.h"
#include "showUI.h"
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include "parseMessage.h"
using json = nlohmann::json;
User g_currentUser;
std::vector<User> g_friendVec;
std::vector<Group> g_groupVec;
int main()
{
    ClientNet client;
    ParseMessage parse;
    client.connect();
    showUI ui;
    int n = 1;
    while(n != 0)
    {
        n = ui.showStart();
        switch (n)
        {
        case 0:
            break;
        case 1:
            {
                std::string sendstr = ui.showLogin();
                if(client.send(sendstr))
                {
                    std::string resmsg = client.recvmsg();
                    if(resmsg == "")
                    {
                        continue;
                    }
                    parse.parseLogin(resmsg,g_currentUser,g_friendVec,g_groupVec);
                    
                }else
                {
                    std::cout << "发送失败" << std::endl;
                    break;
                }
                ui.showUserMain(g_currentUser,g_friendVec,g_groupVec);
            }
            break;
        default:
            break;
        }
    }
    return 0;
}