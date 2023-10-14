/////////////////////////////////////////////////////////////////////
/// file: DatagramSocket.h
///
/// summary: UDP socket
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_DATAGRAMSOCKET_H
#define ICENFSD_DATAGRAMSOCKET_H

#include <WinSock2.h>
#include <cstdint>
#include <string>

class Socket;
class ISocketListener;

class DatagramSocket
{
public:
	DatagramSocket(const sockaddr_in& endpoint, ISocketListener* listener);
	~DatagramSocket();

	const std::string& GetAddress() const noexcept;
	void Close();

private:
	std::string m_address;
	SOCKET m_socket;
	Socket* m_pSocket;
	bool m_closed;
	ISocketListener* m_listener;
};

#endif // ICENFSD_DATAGRAMSOCKET_H
