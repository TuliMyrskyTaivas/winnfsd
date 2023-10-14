/////////////////////////////////////////////////////////////////////
/// file: SocketStream.h
///
/// summary: stream I/O for the socket
/////////////////////////////////////////////////////////////////////

#include "SocketStream.h"
#include <sys/types.h>
#include <cstring>
#include <cstdio>

#define MAXDATA (1024 * 1024)

/////////////////////////////////////////////////////////////////////
SocketStream::SocketStream()
	: m_inBuffer(new unsigned char[MAXDATA])
	, m_outBuffer(new unsigned char[MAXDATA])
	, m_inBufferSize(0)
	, m_outBufferSize(0)
	, m_inBufferIndex(0)
	, m_outBufferIndex(0)
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
void SocketStream::SetInputSize(size_t size)
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
size_t SocketStream::GetOutputSize() const noexcept
{
	return m_outBufferSize;  //number of bytes of data in the output buffer
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::GetBufferSize() const noexcept
{
	return MAXDATA;  //size of input/output buffer
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::Read(void* data, size_t size)
{
	if (size > m_inBufferSize - m_inBufferIndex) { //over the number of bytes of data in the input buffer
		size = m_inBufferSize - m_inBufferIndex;
	}

	memcpy(data, m_inBuffer + m_inBufferIndex, size);
	m_inBufferIndex += static_cast<off_t>(size);

	return size;
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::Read(uint32_t* value)
{
	constexpr size_t bufferLength = sizeof(uint32_t);
	unsigned char buffer[bufferLength];

	unsigned char* p = reinterpret_cast<unsigned char*>(value);
	const auto n = Read(buffer, bufferLength);

	for (auto i = 0; i < n; i++) // reverse byte order
	{
		p[bufferLength - 1 - i] = buffer[i];
	}

	return n;
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::Read8(uint64_t* value)
{
	constexpr size_t bufferLength = sizeof(uint64_t);
	unsigned char buffer[bufferLength];

	unsigned char* p = (unsigned char*)value;
	const auto n = Read(buffer, bufferLength);

	for (auto i = 0; i < n; i++) { //reverse byte order
		p[bufferLength - 1 - i] = buffer[i];
	}

	return n;
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::Skip(size_t size)
{
	if (size > m_inBufferSize - m_inBufferIndex) //over the number of bytes of data in the input buffer
	{
		size = m_inBufferSize - m_inBufferIndex;
	}

	m_inBufferIndex += static_cast<off_t>(size);
	return size;
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::GetSize() const noexcept
{
	return m_inBufferSize - m_inBufferIndex;  //number of bytes of rest data in the input buffer
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write(void* data, size_t size)
{
	if (m_outBufferIndex + size > MAXDATA) //over the size of output buffer
	{
		size = MAXDATA - m_outBufferIndex;
	}

	memcpy(m_outBuffer + m_outBufferIndex, data, size);
	m_outBufferIndex += static_cast<off_t>(size);

	if (m_outBufferIndex > m_outBufferSize)
	{
		m_outBufferSize = m_outBufferIndex;
	}
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write(uint32_t value)
{
	constexpr int32_t bufferLength = sizeof(uint32_t);
	unsigned char buffer[bufferLength];
	unsigned char* p = (unsigned char*)&value;

	for (int32_t i = bufferLength - 1; i >= 0; i--) //reverse byte order
	{
		buffer[i] = p[bufferLength - 1 - i];
	}

	Write(buffer, bufferLength);
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Write8(uint64_t value)
{
	constexpr int32_t bufferLength = sizeof(uint64_t);
	unsigned char buffer[bufferLength];
	unsigned char* p = (unsigned char*)&value;

	for (int32_t i = bufferLength - 1; i >= 0; i--) //reverse byte order
	{
		buffer[i] = p[bufferLength - 1 - i];
	}

	Write(buffer, bufferLength);
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Seek(off_t offset, int from)
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
		m_outBufferIndex = static_cast<off_t>(m_outBufferSize) + offset;
	}
}

/////////////////////////////////////////////////////////////////////
size_t SocketStream::GetPosition() const noexcept
{
	return m_outBufferIndex;
}

/////////////////////////////////////////////////////////////////////
void SocketStream::Reset()
{
	m_outBufferIndex = 0;
	m_outBufferSize = 0;  //clear output buffer
}