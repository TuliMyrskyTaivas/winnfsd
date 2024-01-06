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
	/// <summary> Write buffer of specified length to the stream </summary>
	/// <param name="data"> Pointer to the buffer </param>
	/// <param name="size"> Amount of bytes to write </param>
	virtual void Write(void* data, size_t size) = 0;
	/// <summary> Write uint32_t value to the stream </summary>
	/// <param name="value"> A value to write </param>
	virtual void Write(uint32_t value) = 0;
	/// <summary> Write uint64_t value to the stream </summary>
	/// <param name="value"> A value to write </param>
	virtual void Write8(uint64_t value) = 0;
	/// <summary> Set writing position in the stream </summary>
	/// <param name="offset"> Offset to set </param>
	/// <param name="from"> Direction to count from </param>
	virtual void Seek(off_t offset, int from) = 0;
	/// <summary> Get current position in the stream </summary>
	/// <returns> Offset in stream </returns>
	virtual size_t GetPosition() const noexcept = 0;
};

#endif
