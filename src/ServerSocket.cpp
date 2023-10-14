/////////////////////////////////////////////////////////////////////
/// file: ServerSocket.h
///
/// summary: Listening socket
/////////////////////////////////////////////////////////////////////

#include "ServerSocket.h"
#include <process.h>
#include <WS2tcpip.h>
#include <boost/algorithm/string/trim.hpp>

#include <stdexcept>
#include <cassert>

/////////////////////////////////////////////////////////////////////
ServerSocket::ServerSocket(const sockaddr_in& endpoint, int maxClients, ISocketListener* listener)
	: m_closed(false)
	, m_address(16, ' ')
	, m_serverSocket(socket(AF_INET, SOCK_STREAM, 0))
	, m_listener(listener)
{
	inet_ntop(AF_INET, &endpoint.sin_addr, m_address.data(), 16);
	boost::algorithm::trim_right(m_address);
	m_address += ":" + std::to_string(ntohs(endpoint.sin_port)) + "(tcp)";

	if (m_serverSocket == INVALID_SOCKET)
	{
		throw std::runtime_error("failed to create TCP socket");
	}

	if (bind(m_serverSocket, (struct sockaddr*)&endpoint, sizeof(endpoint)) == SOCKET_ERROR)
	{
		closesocket(m_serverSocket);
		throw std::runtime_error("failed to bind TCP socket at " + m_address);
	}

	if (listen(m_serverSocket, maxClients) == SOCKET_ERROR)
	{
		closesocket(m_serverSocket);
		throw std::runtime_error("failed to listen TCP socket at " + m_address);
	}

	m_sockets.reserve(maxClients);
	for (int i = 0; i < maxClients; i++)
	{
		m_sockets.emplace_back(std::make_unique<Socket>(SOCK_STREAM));
	}

	m_thread = std::thread(&ServerSocket::Run, this);
}

/////////////////////////////////////////////////////////////////////
ServerSocket::~ServerSocket()
{
	Close();
}

/////////////////////////////////////////////////////////////////////
void ServerSocket::Close(void)
{
	if (m_closed)
	{
		return;
	}

	m_closed = true;

	closesocket(m_serverSocket);
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	m_sockets.clear();
}

/////////////////////////////////////////////////////////////////////
const std::string& ServerSocket::GetAddress() const noexcept
{
	return m_address;
}

/////////////////////////////////////////////////////////////////////
void ServerSocket::Run()
{
	struct sockaddr_in remoteAddr {};
	int size = sizeof(remoteAddr);

	while (!m_closed)
	{
		SOCKET endpoint = accept(m_serverSocket, (struct sockaddr*)&remoteAddr, &size);  //accept connection

		if (endpoint != INVALID_SOCKET)
		{
			for (auto& socket : m_sockets)
			{
				if (!socket->Active()) //find an inactive Socket
				{
					socket->Open(endpoint, m_listener, &remoteAddr);  //receive input data
					break;
				}
			}
		}
	}
}