#include <iostream>
#include "clientNet.h"
#include <string>
#include "json.hpp"
#include "public.h"
#include "showUI.h"
#include "user.h"
#include "group.h"
#include "groupuser.h"
#include <thread>
#include "Logger.h"

int main(int args,char**argv)
{
    showUI ui;
    Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    int n = 1;
    while (n != 0)
    {
        n = ui.showStart();
        switch (n)
        {
        case 0:
            break;
        case 1:
        {
            bool res = ui.showLogin();
            if(!res)
            {
                break;
            }
            ui.showUserMain();
            ui.showofflineMsg();
            int type = ui.showChatMain();
            std::cout << "type:" << type << std::endl;
            if(type == -1)
            {
                return 0;
            }
        }
        break;
        case 2:
        {
            ui.showRegister();
        }
        break;
        default:
            break;
        }
    }
    return 0;
}