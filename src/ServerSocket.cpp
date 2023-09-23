/////////////////////////////////////////////////////////////////////
/// file: ServerSocket.h
///
/// summary: Listening socket
/////////////////////////////////////////////////////////////////////

#include "ServerSocket.h"
#include <process.h>
#include <cassert>

/////////////////////////////////////////////////////////////////////
static unsigned int __stdcall ThreadProc(void *parameter)
{
    reinterpret_cast<ServerSocket*>(parameter)->Run();
    return 0;
}

/////////////////////////////////////////////////////////////////////
ServerSocket::ServerSocket()
    : m_port(0)
    , m_maxNum(0)
    , m_closed(true)
    , m_serverSocket(INVALID_SOCKET)
    , m_thread(INVALID_HANDLE_VALUE)
    , m_listener(nullptr)
    , m_sockets(nullptr)
{}

/////////////////////////////////////////////////////////////////////
ServerSocket::~ServerSocket()
{
    Close();
}

/////////////////////////////////////////////////////////////////////
void ServerSocket::SetListener(ISocketListener *listener)
{
    m_listener = listener;
}

/////////////////////////////////////////////////////////////////////
bool ServerSocket::Open(int port, int maxNum)
{
    Close();

    m_port = port;
    m_maxNum = maxNum;  //max number of concurrent clients
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_serverSocket == INVALID_SOCKET)
    {
        return false;
    }

    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(static_cast<u_short>(m_port));
	localAddr.sin_addr.s_addr = inet_addr(g_sInAddr);
	if (localAddr.sin_addr.s_addr == INADDR_NONE)
    {
		g_sInAddr = "0.0.0.0";
		localAddr.sin_addr.s_addr = INADDR_ANY;
	}

    if (bind(m_serverSocket, (struct sockaddr *)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
    {
        closesocket(m_serverSocket);
        return false;
    }

    if (listen(m_serverSocket, m_maxNum) == SOCKET_ERROR)
    {
        closesocket(m_serverSocket);
        return false;
    }

    m_sockets = new Socket*[m_maxNum];

    for (int i = 0; i < m_maxNum; i++)
    {
        m_sockets[i] = new Socket(SOCK_STREAM);
    }

    m_closed = false;
    unsigned int id;
    m_thread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, &id);  //begin thread

    return true;
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

    if (m_thread != nullptr)
    {
        WaitForSingleObject(m_thread, INFINITE);
        CloseHandle(m_thread);
    }

    if (m_sockets != nullptr)
    {
        for (int i = 0; i < m_maxNum; i++)
        {
            delete m_sockets[i];
        }

        delete[] m_sockets;
        m_sockets = nullptr;
    }
}

/////////////////////////////////////////////////////////////////////
int ServerSocket::GetPort() const noexcept
{
    return m_port;
}

/////////////////////////////////////////////////////////////////////
void ServerSocket::Run()
{
    struct sockaddr_in remoteAddr;

    int size = sizeof(remoteAddr);

    while (!m_closed)
    {
        SOCKET socket = accept(m_serverSocket, (struct sockaddr *)&remoteAddr, &size);  //accept connection

        if (socket != INVALID_SOCKET)
        {
            for (int i = 0; i < m_maxNum; i++)
            {
                if (!m_sockets[i]->Active()) //find an inactive CSocket
                {
                    m_sockets[i]->Open(socket, m_listener, &remoteAddr);  //receive input data
                    break;
                }
            }
        }

    }
}
