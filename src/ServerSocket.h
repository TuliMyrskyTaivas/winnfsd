/////////////////////////////////////////////////////////////////////
/// file: ServerSocket.h
///
/// summary: Listening socket
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_SERVERSOCKET_H
#define ICENFSD_SERVERSOCKET_H

#include "SocketListener.h"
#include "Socket.h"
#include <winsock.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <thread>

class ServerSocket
{
public:
	ServerSocket(const sockaddr_in& endpoint, int maxClients, ISocketListener* listener);
	~ServerSocket();

	const std::string& GetAddress() const noexcept;
	void Close();
	void Run();

private:
	bool m_closed;
	std::string m_address;
	SOCKET m_serverSocket;
	ISocketListener* m_listener;
	std::thread m_thread;
	std::vector<std::unique_ptr<Socket>> m_sockets;
};

#endif // ICENFSD_SERVERSOCKET_H
