/////////////////////////////////////////////////////////////////////
/// file: DatagramSocket.cpp
///
/// summary: UDP socket
/////////////////////////////////////////////////////////////////////

#include "DatagramSocket.h"
#include "Socket.h"

/////////////////////////////////////////////////////////////////////
DatagramSocket::DatagramSocket()
    : m_port(0)
    , m_socket(INVALID_SOCKET)
    , m_pSocket(nullptr)
    , m_closed(true)
    , m_listener(nullptr)
{}

/////////////////////////////////////////////////////////////////////
DatagramSocket::~DatagramSocket()
{
    Close();
}

/////////////////////////////////////////////////////////////////////
void DatagramSocket::SetListener(ISocketListener* listener)
{
    m_listener = listener;
}

/////////////////////////////////////////////////////////////////////
bool DatagramSocket::Open(int port)
{
    struct sockaddr_in localAddr;

    Close();

    m_port = port;
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }

    const int sndBuffer = 1024 * 1024 * 1;
    const int sndBufferSize = (int)sizeof(sndBuffer);
    setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sndBuffer, sndBufferSize);
    const int rcvBuffer = 1024 * 1024 * 8;
    const int rcvBufferSize = (int)sizeof(rcvBuffer);
    setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvBuffer, rcvBufferSize);

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(static_cast<u_short>(m_port));
	localAddr.sin_addr.s_addr = inet_addr(g_sInAddr);
	if (localAddr.sin_addr.s_addr == INADDR_NONE)
    {
		g_sInAddr = "0.0.0.0";
		localAddr.sin_addr.s_addr = INADDR_ANY;
	}

    if (bind(m_socket, (struct sockaddr *)&localAddr, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        closesocket(m_socket);
        return false;
    }

    m_closed = false;
    m_pSocket = new Socket(SOCK_DGRAM);
    m_pSocket->Open(m_socket, m_listener);  //wait for receiving data
    return true;
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

/////////////////////////////////////////////////////////////////////
int DatagramSocket::GetPort() const noexcept
{
    return m_port;
}
