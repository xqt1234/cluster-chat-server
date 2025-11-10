#pragma once
#include <unordered_map>
#include <vector>
#include "Channel.h"

/**
 * @brief 主要思路就是封装一下对epoll的操作。创建epoll句柄，提供读取该epoll数上的事件列表功能
 * 
 */
class EPollPoller
{
public:
    using ChannelList = std::vector<Channel*>;
    using EventList =std::vector<struct epoll_event>;
    using ChannelMap = std::unordered_map<int,Channel*>;
private:
    //ChannelMap m_clients;/**对于epoll没什么用，可以删掉 */
    int m_epollfd;
    EventList m_events;
    static const int kInitEventListSize = 16;
public:
    EPollPoller(/* args */);
    ~EPollPoller();
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel) const;
    void fillActiveChannels(int numEvents,
                                     ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void update(int option,Channel* channel);
};
