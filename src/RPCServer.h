/////////////////////////////////////////////////////////////////////
/// file: RPCServer.h
///
/// summary: RPC server implementation
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_RPCSERVER_H
#define ICENFSD_RPCSERVER_H

#include "SocketListener.h"
#include "Socket.h"
#include "RPCProg.h"
#include <windows.h>

#define PROG_NUM 10

class RPCServer : public ISocketListener
{
public:
    RPCServer();
    virtual ~RPCServer();

    void Set(int progNumber, RPCProg *progHandle);
    void EnableLog(bool enableLog);
    void SocketReceived(Socket* socket);

protected:
    RPCProg* m_progTable[PROG_NUM];
    HANDLE m_mutex;

    int Process(int type, IInputStream* inStream, IOutputStream* outStream, char* remoteAddr);
};

#endif // ICENFSD_RPCSERVER_H
