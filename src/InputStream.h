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
	/// <summary> Read specified amount of data from stream </summary>
	/// <param name="data"> Pointer to the buffer to store data </param>
	/// <param name="size"> Amount of bytes to read from the stream </param>
	/// <returns> Amount of bytes actually read </returns>
	virtual size_t Read(void* data, size_t size) = 0;
	/// <summary> Read uint32_t (little-endian) value from the stream</summary>
	/// <param name="value"> Pointer to the buffer to store data </param>
	/// <returns> Amount of bytes actually read (have to be 4) </returns>
	virtual size_t Read(uint32_t* value) = 0;
	/// <summary> Read uint64_t (little-endian) value from the stream </summary>
	/// <param name="value"> Pointer to the buffer to store data</param>
	/// <returns> Amount of bytes actually read (have to be 8) </returns>
	virtual size_t Read8(uint64_t* value) = 0;
	/// <summary> Skip specified amount of bytes in the stream </summary>
	/// <param name="size"> Amount of bytes to skip </param>
	/// <returns> Amount of bytes actually skipped </returns>
	virtual size_t Skip(size_t size) = 0;
	/// <summary> Get amount of bytes to read in the stream </summary>
	/// <returns> Amount of bytes available in the stream to read </returns>
	virtual size_t GetSize() const noexcept = 0;
};

#endif // ICENFSD_INPUTSTREAM_H
