#pragma once
#include <vector>
#include <string>
/**
 * @brief 主要思路就是以环的方式使用一段内存，前面8个字节为预留，
 * readindex和writeindex中间为可读数据，写入数据writeindex增加，读出去了readindex增加，
 * 
 */
class Buffer
{
private:
    static const int kCheapPrepend{8};
    static const int kInitialSize{1024};
    std::vector<char> m_buf;
    size_t m_readIndex;
    size_t m_writeIndex;
public:
    Buffer(int initialsize = kInitialSize);
    ~Buffer();
    int readFd(int fd,int *saveErrno);
    size_t writeableBytes();
    size_t prependableBytes();
    size_t readableBytes();
    std::string readAsString(int len);
    std::string readAllAsString();
    void append(const char* src,int len);
private:
    char* begin(){return &(*m_buf.begin());}
    
    void makeSpace(size_t len);
};
