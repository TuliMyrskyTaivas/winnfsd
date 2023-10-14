/////////////////////////////////////////////////////////////////////
/// file: winnfsd.cpp
///
/// summary: entry point
/////////////////////////////////////////////////////////////////////

#include "Socket.h"
#include "RPCServer.h"
#include "PortmapProg.h"
#include "NFSProg.h"
#include "MountProg.h"
#include "ServerSocket.h"
#include "DatagramSocket.h"
#include "Settings.h"
#include <cstdlib>
#include <memory>
#include <vector>

#include <boost/log/trivial.hpp>

#define SOCKET_NUM 3
enum
{
	PORTMAP_PORT = 111,
	MOUNT_PORT = 1058,
	NFS_PORT = 2049
};
enum
{
	PROG_PORTMAP = 100000,
	PROG_NFS = 100003,
	PROG_MOUNT = 100005
};

class WinSockHolder
{
public:
	WinSockHolder()
	{
		WSADATA wsaData;
		switch (WSAStartup(0x0202, &wsaData))
		{
		case WSASYSNOTREADY:
			throw std::runtime_error("The underlying network subsystem is not ready for network communication");
		case WSAVERNOTSUPPORTED:
			throw std::runtime_error("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation");
		case WSAEINPROGRESS:
			throw std::runtime_error("A blocking Windows Sockets 1.1 operation is in progress");
		case WSAEPROCLIM:
			throw std::runtime_error("A limit on the number of tasks supported by the Windows Sockets implementation has been reached");
		}
		BOOST_LOG_TRIVIAL(debug) << "Using " << wsaData.szDescription;
	}

	~WinSockHolder()
	{
		WSACleanup();
	}
};

/////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
	const Settings settings(argc, argv);
	const WinSockHolder winsock{};

	DatagramSocket DatagramSockets[SOCKET_NUM];
	ServerSocket ServerSockets[SOCKET_NUM];

	auto rpcServer = std::make_unique<RPCServer>();
	auto portMapper = std::make_unique<PortmapProg>();
	auto nfsServer = std::make_unique<NFSProg>();
	auto mountServer = std::make_unique<MountProg>();

	portMapper->Set(PROG_MOUNT, MOUNT_PORT);  // map port for mount
	portMapper->Set(PROG_NFS, NFS_PORT);      // map port for nfs
	nfsServer->SetUserID(settings.GetUid(), settings.GetGid());  //set uid and gid of files

	for (const auto& mountPoint : settings.GetExports())
	{
		mountServer->Export(mountPoint.first.c_str(), mountPoint.second.c_str());
	}

	rpcServer->Set(PROG_PORTMAP, std::move(portMapper));  //program for portmap
	rpcServer->Set(PROG_NFS, std::move(nfsServer));       //program for nfs
	rpcServer->Set(PROG_MOUNT, std::move(mountServer));   //program for mount

	for (int i = 0; i < SOCKET_NUM; i++)
	{
		DatagramSockets[i].SetListener(rpcServer.get());
		ServerSockets[i].SetListener(rpcServer.get());
	}

	if (!ServerSockets[0].Open(PORTMAP_PORT, 3) || !DatagramSockets[0].Open(PORTMAP_PORT))
	{
		throw std::runtime_error("portmap daemon failed to start");
	}
	BOOST_LOG_TRIVIAL(debug) << "Portmap daemon started";

	if (!ServerSockets[1].Open(NFS_PORT, 10) || !DatagramSockets[1].Open(NFS_PORT))
	{
		throw std::runtime_error("NFS daemon failed to start");
	}
	BOOST_LOG_TRIVIAL(debug) << "NFS daemon started";

	if (!ServerSockets[2].Open(MOUNT_PORT, 3) || !DatagramSockets[2].Open(MOUNT_PORT))
	{
		throw std::runtime_error("Mount daemon failed to start");
	}
	BOOST_LOG_TRIVIAL(debug) << "Mount daemon started\n";

	for (int i = 0; i < SOCKET_NUM; i++)
	{
		DatagramSockets[i].Close();
		ServerSockets[i].Close();
	}

	return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
	BOOST_LOG_TRIVIAL(fatal) << e.what() << std::endl;
	return EXIT_FAILURE;
}