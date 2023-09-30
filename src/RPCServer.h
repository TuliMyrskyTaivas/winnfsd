/////////////////////////////////////////////////////////////////////
/// file: RPCServer.h
///
/// summary: RPC server implementation
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_RPCSERVER_H
#define ICENFSD_RPCSERVER_H

#include "SocketListener.h"
#include <memory>
#include <mutex>
#include <map>

class RPCProg;
class Socket;

class RPCServer : public ISocketListener
{
public:
	using RPCProgPtr = std::unique_ptr<RPCProg>;

	RPCServer() = default;
	virtual ~RPCServer();

	void Set(uint32_t progNumber, RPCProgPtr progHandle);
	RPCProg& Get(uint32_t progNumber);
	void EnableLog(bool enableLog);
	void SocketReceived(Socket* socket);

protected:
	std::map<uint32_t, RPCProgPtr> m_progTable;
	std::mutex m_lock;

	int Process(Socket* socket);
};

#endif // ICENFSD_RPCSERVER_H
