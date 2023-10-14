/////////////////////////////////////////////////////////////////////
/// file: RPCServer.h
///
/// summary: RPC server implementation
/////////////////////////////////////////////////////////////////////

#include "RPCServer.h"
#include "ServerSocket.h"
#include "RPCProg.h"
#include "Socket.h"

#include <boost/log/trivial.hpp>
#include <string>

#define MIN_PROG_NUM 100000
enum
{
	CALL = 0,
	REPLY = 1
};

enum
{
	MSG_ACCEPTED = 0,
	MSG_DENIED = 1
};

enum
{
	SUCCESS = 0,
	PROG_UNAVAIL = 1,
	PROG_MISMATCH = 2,
	PROC_UNAVAIL = 3,
	GARBAGE_ARGS = 4
};

struct OpaqueAuth
{
	uint32_t flavor;
	uint32_t length;
};

struct RpcHeader
{
	uint32_t header;
	uint32_t xid;
	uint32_t msg;
	uint32_t rpcvers;
	uint32_t prog;
	uint32_t vers;
	uint32_t proc;
	OpaqueAuth cred;
	OpaqueAuth verf;
};

/////////////////////////////////////////////////////////////////////
RPCServer::~RPCServer()
{}

/////////////////////////////////////////////////////////////////////
void RPCServer::Set(uint32_t progNum, RPCProgPtr progHandle)
{
	const auto result = m_progTable.emplace(progNum, std::move(progHandle));
	if (!result.second)
	{
		throw std::runtime_error("RPC program #" + std::to_string(progNum) + " already exists");
	}
}

/////////////////////////////////////////////////////////////////////
RPCProg& RPCServer::Get(uint32_t progNum)
{
	auto prog = m_progTable.find(progNum);
	if (m_progTable.end() == prog)
	{
		throw std::runtime_error("RPC program " + std::to_string(progNum) + " does not exist");
	}
	return *(prog->second);
}

/////////////////////////////////////////////////////////////////////
void RPCServer::SocketReceived(Socket* socket)
{
	std::scoped_lock<std::mutex> lock(m_lock);
	auto& inStream = socket->GetInputStream();

	while (inStream.GetSize() > 0)
	{
		const int result = Process(socket);  //process input data
		socket->Send();  //send response

		if (result != PRC_OK || socket->GetType() == SOCK_DGRAM)
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////
int RPCServer::Process(Socket* socket)
{
	const int type = socket->GetType();
	auto& inStream = socket->GetInputStream();
	auto& outStream = socket->GetOutputStream();
	const auto remoteAddr = socket->GetRemoteAddress();
	RpcHeader header{};
	RPCParam param{};
	size_t pos = 0, size = 0;
	int result = PRC_OK;

	if (type == SOCK_STREAM)
	{
		inStream.Read(&header.header);
	}

	inStream.Read(&header.xid);
	inStream.Read(&header.msg);
	inStream.Read(&header.rpcvers);    // rpc version
	inStream.Read(&header.prog);       // program
	inStream.Read(&header.vers);       // program version
	inStream.Read(&header.proc);       // procedure
	inStream.Read(&header.cred.flavor);
	inStream.Read(&header.cred.length);
	inStream.Skip(header.cred.length);
	inStream.Read(&header.verf.flavor); // verifier

	BOOST_LOG_TRIVIAL(debug) << "RPC from " << remoteAddr
		<< ": xid:" << std::hex << header.xid
		<< ", msg:" << std::hex << header.msg
		<< ", rpcVers:" << header.rpcvers
		<< ", prog: " << header.prog
		<< ", progVers: " << header.vers
		<< ", proc: " << header.proc;

	if (inStream.Read(&header.verf.length) < sizeof(header.verf.length))
	{
		BOOST_LOG_TRIVIAL(error) << "failed to read verf length";
		result = PRC_FAIL;
	}

	if (inStream.Skip(header.verf.length) < header.verf.length)
	{
		BOOST_LOG_TRIVIAL(error) << "failed to skip " << header.verf.length << " verf bytes";
		result = PRC_FAIL;
	}

	if (type == SOCK_STREAM)
	{
		pos = outStream.GetPosition();   // remember current position
		outStream.Write(header.header);  // this value will be updated later
	}

	outStream.Write(header.xid);
	outStream.Write(REPLY);
	outStream.Write(MSG_ACCEPTED);
	outStream.Write(header.verf.flavor);
	outStream.Write(header.verf.length);

	const auto prog = m_progTable.find(header.prog);

	if (result == PRC_FAIL) // input data is truncated
	{
		outStream.Write(GARBAGE_ARGS);
	}
	else if (m_progTable.cend() == prog) // program is unavailable
	{
		BOOST_LOG_TRIVIAL(error) << "RPC program " << header.prog << " not found";
		outStream.Write(PROG_UNAVAIL);
	}
	else
	{
		outStream.Write(SUCCESS);  // this value may be modified later if process failed
		param.version = header.vers;
		param.procNum = header.proc;
		param.remoteAddr = remoteAddr;
		result = prog->second->Process(inStream, outStream, param);  //process rest input data by program

		if (result == PRC_NOTIMP)   // procedure is not implemented
		{
			outStream.Seek(-4, SEEK_CUR);
			outStream.Write(PROC_UNAVAIL);
		}
		else if (result == PRC_FAIL) // input data is truncated
		{
			outStream.Seek(-4, SEEK_CUR);
			outStream.Write(GARBAGE_ARGS);
		}
	}

	if (type == SOCK_STREAM)
	{
		size = outStream.GetPosition();                    // remember current position
		outStream.Seek(static_cast<off_t>(pos), SEEK_SET); // seek to the position of head
		header.header = static_cast<uint32_t>(0x80000000 + size - pos - 4); // size of output data
		outStream.Write(header.header);                    // update header
	}

	return result;
}