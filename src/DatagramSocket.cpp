/////////////////////////////////////////////////////////////////////
/// file: DatagramSocket.cpp
///
/// summary: UDP socket
/////////////////////////////////////////////////////////////////////

#include "DatagramSocket.h"
#include "Socket.h"

#include <boost/algorithm/string/trim.hpp>
#include <WS2tcpip.h>
#include <stdexcept>

/////////////////////////////////////////////////////////////////////
DatagramSocket::DatagramSocket(const sockaddr_in& endpoint, ISocketListener* listener)
	: m_address(17, ' ')
	, m_socket(socket(AF_INET, SOCK_DGRAM, 0))
	, m_pSocket(nullptr)
	, m_closed(true)
	, m_listener(listener)
{
	if (m_socket == INVALID_SOCKET)
	{
		throw std::runtime_error("failed to create UDP socket");
	}

	inet_ntop(AF_INET, &endpoint.sin_addr, m_address.data(), 16);
	boost::algorithm::trim_right(m_address);
	m_address += ":" + std::to_string(ntohs(endpoint.sin_port)) + "(udp)";

	const int sndBuffer = 1024 * 1024 * 1;
	const int sndBufferSize = (int)sizeof(sndBuffer);
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sndBuffer, sndBufferSize);
	const int rcvBuffer = 1024 * 1024 * 8;
	const int rcvBufferSize = (int)sizeof(rcvBuffer);
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvBuffer, rcvBufferSize);

	if (bind(m_socket, (struct sockaddr*)&endpoint, sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		throw std::runtime_error("failed to bind UDP socket at " + m_address);
	}

	m_closed = false;
	m_pSocket = new Socket(SOCK_DGRAM);
	m_pSocket->Open(m_socket, m_listener);  //wait for receiving data
}

/////////////////////////////////////////////////////////////////////
DatagramSocket::~DatagramSocket()
{
	Close();
}

/////////////////////////////////////////////////////////////////////
const std::string& DatagramSocket::GetAddress() const noexcept
{
	return m_address;
}

/////////////////////////////////////////////////////////////////////
void DatagramSocket::Close()
{
	if (m_closed)
	{
		return;
	}

	m_closed = true;
	delete m_pSocket;
}