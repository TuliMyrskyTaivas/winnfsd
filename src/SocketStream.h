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
	void SetInputSize(size_t size);
	unsigned char* GetOutput() noexcept;
	size_t GetOutputSize() const noexcept;
	size_t GetBufferSize() const noexcept;
	void Reset();

	// IInputStream implementation
	size_t Read(void* data, size_t size) override;
	size_t Read(uint32_t* value) override;
	size_t Read8(uint64_t* value) override;
	size_t Skip(size_t size) override;
	size_t GetSize() const noexcept override;

	// IOutputStream implementation
	void Write(void* data, size_t size) override;
	void Write(uint32_t value) override;
	void Write8(uint64_t value) override;
	void Seek(off_t offset, int from) override;
	size_t GetPosition() const noexcept override;

private:
	unsigned char* m_inBuffer, * m_outBuffer;
	size_t m_inBufferSize, m_outBufferSize;
	off_t m_inBufferIndex, m_outBufferIndex;
};

#endif // ICENFSD_SOCKETSTREAM_H
