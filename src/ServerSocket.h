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

class ServerSocket
{
public:
    ServerSocket();
    ~ServerSocket();

    void SetListener(ISocketListener *listener);
    bool Open(int port, int maxNum);
    void Close();
    int GetPort() const noexcept;
    void Run();

private:
    int m_port, m_maxNum;
    bool m_closed;
    SOCKET m_serverSocket;
    HANDLE m_thread;
    ISocketListener *m_listener;
    Socket **m_sockets;
};

#endif // ICENFSD_SERVERSOCKET_H
