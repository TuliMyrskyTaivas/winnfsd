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
#include "FileTable.h"
#include "ServerSocket.h"
#include "DatagramSocket.h"
#include "Settings.h"
#include <iostream>
#include <cstdlib>
#include <memory>
#include <vector>

#include <boost/log/trivial.hpp>

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

	auto fileTable = std::make_shared<FileTable>();
	auto rpcServer = std::make_unique<RPCServer>();
	auto portMapper = std::make_unique<PortmapProg>();
	auto nfsServer = std::make_unique<NFSProg>(fileTable, settings.GetUid(), settings.GetGid());
	auto mountServer = std::make_unique<MountProg>(fileTable);

	portMapper->Set(PROG_MOUNT, MOUNT_PORT);  // map port for mount
	portMapper->Set(PROG_NFS, NFS_PORT);      // map port for nfs

	for (const auto& mountPoint : settings.GetExports())
	{
		mountServer->Export(mountPoint.first.c_str(), mountPoint.second.c_str());
	}

	rpcServer->Set(PROG_PORTMAP, std::move(portMapper));  //program for portmap
	rpcServer->Set(PROG_NFS, std::move(nfsServer));       //program for nfs
	rpcServer->Set(PROG_MOUNT, std::move(mountServer));   //program for mount

	ServerSocket rpcTcpSocket(settings.GetRpcEndpoint(), 3, rpcServer.get());
	DatagramSocket rpcUdpSocket(settings.GetRpcEndpoint(), rpcServer.get());
	BOOST_LOG_TRIVIAL(debug) << "Portmap daemon started at " << rpcTcpSocket.GetAddress();
	ServerSocket nfsTcpSocket(settings.GetNfsEndpoint(), 10, rpcServer.get());
	DatagramSocket nfsUdpSocket(settings.GetNfsEndpoint(), rpcServer.get());
	BOOST_LOG_TRIVIAL(debug) << "NFS daemon started at " << nfsTcpSocket.GetAddress();
	ServerSocket mountTcpSocket(settings.GetMountEndpoint(), 3, rpcServer.get());
	DatagramSocket mountUdpSocket(settings.GetMountEndpoint(), rpcServer.get());
	BOOST_LOG_TRIVIAL(debug) << "Mount daemon started at " << mountTcpSocket.GetAddress();

	std::string data;
	std::cin >> data;

	return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
	BOOST_LOG_TRIVIAL(fatal) << e.what() << std::endl;
	return EXIT_FAILURE;
}