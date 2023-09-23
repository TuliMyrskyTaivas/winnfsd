/////////////////////////////////////////////////////////////////////
/// file: SocketStream.h
///
/// summary: stream I/O for the socket
/////////////////////////////////////////////////////////////////////

#include "SocketStream.h"
#include <cstring>
#include <cstdio>

#define MAXDATA (1024 * 1024)

/////////////////////////////////////////////////////////////////////
SocketStream::SocketStream()
    : m_inBuffer(new unsigned char[MAXDATA])
    , m_outBuffer(new unsigned char[MAXDATA])
    , m_inBufferIndex(0)
    , m_inBufferSize(0)
    , m_outBufferIndex(0)
    , m_outBufferSize(0)
{}

/////////////////////////////////////////////////////////////////////
SocketStream::~SocketStream()
{
    delete[] m_inBuffer;
    delete[] m_outBuffer;
}

/////////////////////////////////////////////////////////////////////
unsigned char* SocketStream::GetInput() noexcept
{
    return m_inBuffer;
}

/////////////////////////////////////////////////////////////////////
void SocketStream::SetInputSize(unsigned int size)
{
    m_inBufferIndex = 0;  //seek to the beginning of the input buffer
    m_inBufferSize = size;
}

/////////////////////////////////////////////////////////////////////
unsigned char* SocketStream::GetOutput() noexcept
{
    return m_outBuffer;  //output buffer
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::GetOutputSize() const noexcept
{
    return m_outBufferSize;  //number of bytes of data in the output buffer
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::GetBufferSize() const noexcept
{
    return MAXDATA;  //size of input/output buffer
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::Read(void *data, unsigned int size)
{
    if (size > m_inBufferSize - m_inBufferIndex) { //over the number of bytes of data in the input buffer
        size = m_inBufferSize - m_inBufferIndex;
    }

    memcpy(data, m_inBuffer + m_inBufferIndex, size);
    m_inBufferIndex += size;

    return size;
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::Read(unsigned long* value)
{
    unsigned char buffer[sizeof(unsigned long)];

    unsigned char* p = reinterpret_cast<unsigned char*>(value);
    const unsigned int n = Read(buffer, sizeof(unsigned long));

    for (unsigned int i = 0; i < n; i++) // reverse byte order
    { 
        p[sizeof(unsigned long) - 1 - i] = buffer[i];
    }

    return n;
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::Read8(unsigned __int64 *value)
{
    unsigned int i, n;
    unsigned char *p, buffer[sizeof(unsigned __int64)];

    p = (unsigned char *)value;
    n = Read(buffer, sizeof(unsigned __int64));

    for (i = 0; i < n; i++) { //reverse byte order
        p[sizeof(unsigned __int64)-1 - i] = buffer[i];
    }

    return n;
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::Skip(unsigned int size)
{
    if (size > m_inBufferSize - m_inBufferIndex) //over the number of bytes of data in the input buffer
    {
        size = m_inBufferSize - m_inBufferIndex;
    }

    m_inBufferIndex += size;

    return size;
}

/////////////////////////////////////////////////////////////////////
unsigned int SocketStream::GetSize() const noexcept
{
    return m_inBufferSize - m_inBufferIndex;  //number of bytes of rest data in the input buffer
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write(void *data, unsigned int size)
{
    if (m_outBufferIndex + size > MAXDATA) //over the size of output buffer
    {
        size = MAXDATA - m_outBufferIndex;
    }

    memcpy(m_outBuffer + m_outBufferIndex, data, size);
    m_outBufferIndex += size;

    if (m_outBufferIndex > m_outBufferSize)
    {
        m_outBufferSize = m_outBufferIndex;
    }

}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write(unsigned long value)
{
    int i;
    unsigned char *p, buffer[sizeof(unsigned long)];

    p = (unsigned char *)&value;

    for (i = sizeof(unsigned long)-1; i >= 0; i--) //reverse byte order
    {
        buffer[i] = p[sizeof(unsigned long)-1 - i];
    }

    Write(buffer, sizeof(unsigned long));
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write8(unsigned __int64 value)
{
    int i;
    unsigned char *p, buffer[sizeof(unsigned __int64)];

    p = (unsigned char *)&value;

    for (i = sizeof(unsigned __int64)-1; i >= 0; i--) //reverse byte order
    {
        buffer[i] = p[sizeof(unsigned __int64)-1 - i];
    }

    Write(buffer, sizeof(unsigned __int64));
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Seek(int offset, int from)
{
    if (from == SEEK_SET)
    {
        m_outBufferIndex = offset;
    }
    else if (from == SEEK_CUR)
    {
        m_outBufferIndex += offset;
    }
    else if (from == SEEK_END)
    {
        m_outBufferIndex = m_outBufferSize + offset;
    }
}

/////////////////////////////////////////////////////////////////////
int SocketStream::GetPosition() const noexcept
{
    return m_outBufferIndex;
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Reset()
{
    m_outBufferIndex = m_outBufferSize = 0;  //clear output buffer
}
