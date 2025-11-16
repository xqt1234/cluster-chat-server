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

int main()
{
    showUI ui;
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
            ui.showChatMain();
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