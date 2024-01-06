/////////////////////////////////////////////////////////////////////
/// file: Socket.h
///
/// summary: base class for the network socket
/////////////////////////////////////////////////////////////////////

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Socket.h"
#include <boost/log/trivial.hpp>

/////////////////////////////////////////////////////////////////////
Socket::Socket(int type)
	: m_type(type)
	, m_socket(INVALID_SOCKET)
	, m_listener(nullptr)
	, m_active(false)
{
	memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
}

/////////////////////////////////////////////////////////////////////
Socket::~Socket()
{
	Close();
}

/////////////////////////////////////////////////////////////////////
int Socket::GetType() const noexcept
{
	return m_type;
}

/////////////////////////////////////////////////////////////////////
void Socket::Open(SOCKET socket, ISocketListener* listener, struct sockaddr_in* remoteAddr)
{
	Close();

	m_socket = socket;
	m_listener = listener;

	if (remoteAddr != nullptr)
	{
		m_remoteAddr = *remoteAddr;
	}

	if (m_socket != INVALID_SOCKET)
	{
		m_active = true;
		m_thread = std::thread(&Socket::Run, this);
	}
}

/////////////////////////////////////////////////////////////////////
void Socket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

/////////////////////////////////////////////////////////////////////
void Socket::Send()
{
	if (m_socket == INVALID_SOCKET)
	{
		return;
	}

	const int outputSize = static_cast<int>(m_socketStream.GetOutputSize());
	if (m_type == SOCK_STREAM)
	{
		send(m_socket, (const char*)m_socketStream.GetOutput(), outputSize, 0);
	}
	else if (m_type == SOCK_DGRAM)
	{
		sendto(m_socket, (const char*)m_socketStream.GetOutput(), outputSize, 0, (struct sockaddr*)&m_remoteAddr, sizeof(struct sockaddr));
	}

	m_socketStream.Reset();  // clear output buffer
}

/////////////////////////////////////////////////////////////////////
bool Socket::Active() const noexcept
{
	return m_active;  // thread is active or not
}

/////////////////////////////////////////////////////////////////////
char* Socket::GetRemoteAddress() const noexcept
{
	return inet_ntoa(m_remoteAddr.sin_addr);
}

/////////////////////////////////////////////////////////////////////
int Socket::GetRemotePort() const noexcept
{
	return htons(m_remoteAddr.sin_port);
}

/////////////////////////////////////////////////////////////////////
IInputStream& Socket::GetInputStream() noexcept
{
	return m_socketStream;
}

/////////////////////////////////////////////////////////////////////
IOutputStream& Socket::GetOutputStream() noexcept
{
	return m_socketStream;
}

/////////////////////////////////////////////////////////////////////
void Socket::Run()
try
{
	int size = sizeof(m_remoteAddr);
	int bytes = 0;

	for (;;)
	{
		if (m_type == SOCK_STREAM)
		{
			// When using tcp we cannot ensure that everything we need is already
			// received. When using RCP over TCP a fragment header is added to
			// work around this. The MSB of the fragment header determines if the
			// fragment is complete (not used here) and the remaining bits define the
			// length of the rpc call (this is what we want)
			bytes = recv(m_socket, (char*)m_socketStream.GetInput(), 4, MSG_PEEK);

			// only if at least 4 bytes are availabe (the fragment header) we can continue
			if (bytes == 4)
			{
				uint32_t fragmentHeader = 0;
				m_socketStream.SetInputSize(4);
				m_socketStream.Read(&fragmentHeader);
				const int fragmentHeaderMsb = (int)(fragmentHeader & 0x80000000);
				const int fragmentHeaderLengthBytes = fragmentHeaderMsb + 4;
				while (bytes != fragmentHeaderLengthBytes)
				{
					bytes = recv(m_socket, (char*)m_socketStream.GetInput(), fragmentHeaderLengthBytes, MSG_PEEK);
				}
				bytes = recv(m_socket, (char*)m_socketStream.GetInput(), fragmentHeaderLengthBytes, 0);
			}
			else
			{
				bytes = 0;
			}
		}
		else if (m_type == SOCK_DGRAM)
		{
			bytes = recvfrom(m_socket, (char*)m_socketStream.GetInput(), static_cast<int>(m_socketStream.GetBufferSize()), 0, (struct sockaddr*)&m_remoteAddr, &size);
		}

		if (bytes > 0)
		{
			m_socketStream.SetInputSize(bytes);  // bytes received

			if (m_listener != nullptr)
			{
				m_listener->SocketReceived(this);  // notify listener
			}
		}
		else
		{
			break;
		}
	}

	m_active = false;
}
catch (const std::exception& e)
{
	BOOST_LOG_TRIVIAL(error) << "socket operation failed: " << e.what();
}