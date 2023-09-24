/////////////////////////////////////////////////////////////////////
/// file: DatagramSocket.h
///
/// summary: UDP socket
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_DATAGRAMSOCKET_H
#define ICENFSD_DATAGRAMSOCKET_H

#include <winsock.h>

class Socket;
class ISocketListener;

class DatagramSocket
{
public:
    DatagramSocket();
    ~DatagramSocket();

    void SetListener(ISocketListener* listener);
    bool Open(int port);
    void Close();
    int GetPort() const noexcept;

private:
    int m_port;
    SOCKET m_socket;
    Socket *m_pSocket;
    bool m_closed;
    ISocketListener* m_listener;
};

#endif // ICENFSD_DATAGRAMSOCKET_H
