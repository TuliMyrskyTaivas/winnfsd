/////////////////////////////////////////////////////////////////////
/// file: OutputStream.h
///
/// summary: abstraction of the output stream
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_OUTPUTSTREAM_H
#define ICENFSD_OUTPUTSTREAM_H

#include <cstdint>
#include <sys/types.h>

class IOutputStream
{
public:
	virtual void Write(void* data, size_t size) = 0;
	virtual void Write(uint32_t value) = 0;
	virtual void Write8(uint64_t value) = 0;
	virtual void Seek(off_t offset, int from) = 0;
	virtual size_t GetPosition() const noexcept = 0;
};

#endif
