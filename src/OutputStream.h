/////////////////////////////////////////////////////////////////////
/// file: OutputStream.h
///
/// summary: abstraction of the output stream
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_OUTPUTSTREAM_H
#define ICENFSD_OUTPUTSTREAM_H

class IOutputStream
{
public:
    virtual void Write(void *data, unsigned int size) = 0;
    virtual void Write(unsigned long value) = 0;
    virtual void Write8(unsigned __int64 value) = 0;
    virtual void Seek(int offset, int from) = 0;
    virtual int GetPosition() const noexcept = 0;
};

#endif
