#ifndef _SOCKETLISTENER_H_
#define _SOCKETLISTENER_H_

class Socket;

class ISocketListener
{
    public:
    virtual void SocketReceived(Socket *pSocket) = 0;
};

#endif
