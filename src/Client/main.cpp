#include <iostream>
#include "ClientNet.h"
#include <string>
using namespace std;
int main()
{
    ClientNet client;
    client.connect();
    int n = 1;
    
    while(n != 0)
    {
        std::cout << "--------请输入------" << std::endl;
        std::cout << "0:退出" << std::endl;
        std::cout << "1:发送消息" << std::endl;
        cin >> n;
        cin.get();
        switch (n)
        {
        case 0:
            break;
        case 1:
            {
                std::string sendstr;
                cin>>sendstr;
                client.send(sendstr);
            }
        default:
            break;
        }
    }
    return 0;
}