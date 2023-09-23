/////////////////////////////////////////////////////////////////////
/// file: InputStream.h
///
/// summary: abstraction of the input stream
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_INPUTSTREAM_H
#define ICENFSD_INPUTSTREAM_H

class IInputStream
{
public:
    virtual unsigned int Read(void *data, unsigned int size) = 0;
    virtual unsigned int Read(unsigned long *value) = 0;
    virtual unsigned int Read8(unsigned __int64 *value) = 0;
    virtual unsigned int Skip(unsigned int size) = 0;
    virtual unsigned int GetSize() const noexcept = 0;
};

#endif // ICENFSD_INPUTSTREAM_H
