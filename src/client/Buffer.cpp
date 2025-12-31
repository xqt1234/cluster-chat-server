#include "Buffer.h"
#include <sys/uio.h>
#include <sys/socket.h>
#include <errno.h>

Buffer::Buffer(int initialsize)
    :m_buf(kCheapPrepend + initialsize)
    , m_readIndex(kCheapPrepend)
    , m_writeIndex(kCheapPrepend)
{
}

Buffer::~Buffer()
{
}

int Buffer::readFd(int fd,int *saveErrno)
{
    char extraBuf[65536]{};
    const size_t twriteable = writeableBytes();
    struct iovec vec[2]{};
    vec[0].iov_base = begin() + m_writeIndex;
    vec[0].iov_len = twriteable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);
    int iovcnt = twriteable > sizeof(extraBuf) ? 1 : 2;
    ssize_t n = readv(fd,vec,iovcnt);
    if(n < 0)
    {
        *saveErrno = errno;
    }else if(n <= twriteable)
    {
        m_writeIndex+= n;
    }else
    {
        m_writeIndex = m_buf.size();
        append(extraBuf,n - twriteable);
    }
    return n;
}

size_t Buffer::writeableBytes()
{
    return m_buf.size() - m_writeIndex;
}

size_t Buffer::prependableBytes()
{
    return m_readIndex;
}

size_t Buffer::readableBytes()
{
    return m_writeIndex - m_readIndex;
}

std::string Buffer::readAsString(int len)
{
    if(len > readableBytes())
    {
        len = readableBytes();
    }
    std::string result(begin() + m_readIndex, len);
    m_readIndex += len;
    return result;
}

std::string Buffer::readAllAsString()
{
    std::string result(begin() + m_readIndex, readableBytes());
    m_readIndex = m_writeIndex;
    return result;
}

void Buffer::retrieve(size_t len)
{
    if(len < readableBytes())
    {
        m_readIndex+=len;
    }else
    {
        retrieveAll();
    }
}

void Buffer::retrieveAll()
{
    m_readIndex = m_writeIndex = kCheapPrepend;
}

void Buffer::append(const char *src, int len)
{
    if(len > writeableBytes())
    {
        makeSpace(len);
    }
    std::copy(src,src + len,begin() + m_writeIndex);
    m_writeIndex+=len;
}

const char *Buffer::peek() const
{
    return begin()+ m_readIndex;
}

void Buffer::makeSpace(size_t len)
{
    if(writeableBytes() + prependableBytes() - kCheapPrepend < len)
    {
        m_buf.resize(m_writeIndex + len);
    }
    else
    {
        int readcnt = readableBytes();
        std::copy(begin() + m_readIndex,begin() + m_writeIndex,begin() + kCheapPrepend);
        m_readIndex = kCheapPrepend;
        m_writeIndex = m_readIndex + readcnt;
    }
}
