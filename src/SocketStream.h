/////////////////////////////////////////////////////////////////////
/// file: SocketStream.h
///
/// summary: stream I/O for the socket
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_SOCKETSTREAM_H
#define ICENFSD_SOCKETSTREAM_H

#include "InputStream.h"
#include "OutputStream.h"

class SocketStream : public IInputStream, public IOutputStream
{
public:
    SocketStream();
    virtual ~SocketStream();

    unsigned char* GetInput() noexcept;
    void SetInputSize(unsigned int size);
    unsigned char* GetOutput() noexcept;
    unsigned int GetOutputSize() const noexcept;
    unsigned int GetBufferSize() const noexcept;
    void Reset();

    // IInputStream implementation
    unsigned int Read(void *data, unsigned int size) override;
    unsigned int Read(unsigned long *value) override;
    unsigned int Read8(unsigned __int64 *value) override;
    unsigned int Skip(unsigned int size) override;
    unsigned int GetSize() const noexcept override;

    // IOutputStream implementation
    void Write(void *data, unsigned int size) override;
    void Write(unsigned long value) override;
    void Write8(unsigned __int64 value) override;
    void Seek(int offset, int from) override;
    int GetPosition() const noexcept override;
    
private:
    unsigned char *m_inBuffer, *m_outBuffer;
    unsigned int m_inBufferIndex, m_inBufferSize, m_outBufferIndex, m_outBufferSize;
};

#endif // ICENFSD_SOCKETSTREAM_H
