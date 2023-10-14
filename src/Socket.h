/////////////////////////////////////////////////////////////////////
/// file: Socket.h
///
/// summary: base class for the network socket
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_SOCKET_H
#define ICENFSD_SOCKET_H

#include "SocketListener.h"
#include "SocketStream.h"
#include <winsock2.h>

class Socket
{
public:
	Socket(int type);
	virtual ~Socket();

	int GetType() const noexcept;
	void Open(SOCKET socket, ISocketListener* listener, struct sockaddr_in* remoteAddr = nullptr);
	void Close();
	void Send();
	bool Active() const noexcept;
	char* GetRemoteAddress() const noexcept;
	int GetRemotePort() const noexcept;
	IInputStream& GetInputStream() noexcept;
	IOutputStream& GetOutputStream() noexcept;
	void Run();

private:
	int m_type;
	SOCKET m_socket;
	struct sockaddr_in m_remoteAddr;
	ISocketListener* m_listener;
	SocketStream m_socketStream;
	bool m_active;
	HANDLE m_thread;
};

#endif // ICENFSD_SOCKET_H
