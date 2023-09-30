/////////////////////////////////////////////////////////////////////
/// file: InputStream.h
///
/// summary: abstraction of the input stream
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_INPUTSTREAM_H
#define ICENFSD_INPUTSTREAM_H

#include <cstdint>

class IInputStream
{
public:
	virtual size_t Read(void* data, size_t size) = 0;
	virtual size_t Read(uint32_t* value) = 0;
	virtual size_t Read8(uint64_t* value) = 0;
	virtual size_t Skip(size_t size) = 0;
	virtual size_t GetSize() const noexcept = 0;
};

#endif // ICENFSD_INPUTSTREAM_H
