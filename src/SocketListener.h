/////////////////////////////////////////////////////////////////////
/// file: SocketListener.h
///
/// summary: interface for socket listener
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_SOCKETLISTENER_H
#define ICENFSD_SOCKETLISTENER_H

class Socket;

class ISocketListener
{
public:
    virtual void SocketReceived(Socket *socket) = 0;
};

#endif // ICENFSD_SOCKETLISTENER_H
