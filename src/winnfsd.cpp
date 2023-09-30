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
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

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

struct MountPoint
{
	std::string path;
	std::string alias;

	MountPoint(const char* path, const char* alias)
		: path(path)
		, alias(alias)
	{}

	MountPoint(std::string&& path, std::string& alias)
		: path(std::move(path))
		, alias(std::move(alias))
	{}
};

/////////////////////////////////////////////////////////////////////
static void PrintUsage(char* exeFilename)
{
	std::cout
		<< "\nUsage: " << exeFilename << " [-id <uid> <gid>] [-log on | off] [-pathFile <file>] [-addr <ip>] [export path] [alias path]\n\n"
		<< "At least a file or a path is needed\n"
		<< "For example:\n"
		<< "On Windows> " << exeFilename << " d:\\work\n"
		<< "On Linux> mount -t nfs 192.168.12.34:/d/work mount\n\n"
		<< "For another example:\n"
		<< "On Windows> " << exeFilename << " d:\\work /exports\n"
		<< "On Linux> mount -t nfs 192.168.12.34:/exports\n\n"
		<< "Another example where WinNFSd is only bound to a specific interface:\n"
		<< "On Windows> " << exeFilename << " -addr 192.168.12.34 d:\\work /exports\n"
		<< "On Linux> mount - t nfs 192.168.12.34: / exports\n\n"
		<< "Use \".\" to export the current directory (works also for -filePath):\n"
		<< "On Windows> " << exeFilename << " . /exports\n";
}

/////////////////////////////////////////////////////////////////////
static void PrintLine()
{
	std::cout << "=====================================================\n";
}

/////////////////////////////////////////////////////////////////////
static void PrintAbout()
{
	PrintLine();
	printf("WinNFSd {{VERSION}} [{{HASH}}]\n");
	printf("Network File System server for Windows\n");
	printf("Copyright (C) 2005 Ming-Yang Kao\n");
	printf("Edited in 2011 by ZeWaren\n");
	printf("Edited in 2013 by Alexander Schneider (Jankowfsky AG)\n");
	printf("Edited in 2014 2015 by Yann Schepens\n");
	printf("Edited in 2016 by Peter Philipp (Cando Image GmbH), Marc Harding\n");
	PrintLine();
}

/////////////////////////////////////////////////////////////////////
static void PrintHelp()
{
	PrintLine();
	printf("Commands:\n");
	printf("about: display messages about this program\n");
	printf("help: display help\n");
	printf("log on/off: display log messages or not\n");
	printf("list: list mounted clients\n");
	printf("refresh: refresh the mounted folders\n");
	printf("reset: reset the service\n");
	printf("quit: quit this program\n");
	PrintLine();
}

/////////////////////////////////////////////////////////////////////
static void PrintCount(const MountProg& mount)
{
	std::cout << "There are " << mount.GetMountNumber() << " clients mounted\n";
}

/////////////////////////////////////////////////////////////////////
static void PrintMounts(const MountProg& mount)
{
	PrintLine();
	const int mounts = mount.GetMountNumber();

	for (int i = 0; i < mounts; i++)
	{
		std::cout << mount.GetClientAddr(i);
	}

	PrintCount(mount);
	PrintLine();
}

/////////////////////////////////////////////////////////////////////
static void PrintConfirmQuit(const MountProg& mount)
{
	printf("\n");
	PrintCount(mount);
	printf("Are you sure to quit? (y/N): ");
}

/////////////////////////////////////////////////////////////////////
static void MountPaths(MountProg& mount, const std::vector<MountPoint>& paths)
{
	for (const auto& mountPoint : paths)
	{
		mount.Export(mountPoint.path.c_str(), mountPoint.alias.c_str());
	}
}

/////////////////////////////////////////////////////////////////////
static void InputCommand(RPCServer* rpc)
{
	MountProg& mount = dynamic_cast<MountProg&>(rpc->Get(PROG_MOUNT));
	char command[20];

	printf("Type 'help' to see help\n\n");

	while (true)
	{
		fgets(command, 20, stdin);

		if (command[strlen(command) - 1] == '\n')
		{
			command[strlen(command) - 1] = '\0';
		}

		if (_stricmp(command, "about") == 0)
		{
			PrintAbout();
		}
		else if (_stricmp(command, "help") == 0)
		{
			PrintHelp();
		}
		else if (_stricmp(command, "log on") == 0)
		{
			rpc->EnableLog(true);
		}
		else if (_stricmp(command, "log off") == 0)
		{
			rpc->EnableLog(false);
		}
		else if (_stricmp(command, "list") == 0)
		{
			PrintMounts(mount);
		}
		else if (_stricmp(command, "quit") == 0)
		{
			if (mount.GetMountNumber() == 0)
			{
				break;
			}
			else
			{
				PrintConfirmQuit(mount);
				fgets(command, 20, stdin);

				if (command[0] == 'y' || command[0] == 'Y')
				{
					break;
				}
			}
		}
		else if (_stricmp(command, "refresh") == 0)
		{
			mount.Refresh();
		}
		else if (_stricmp(command, "reset") == 0)
		{
			rpc->Set(PROG_NFS, nullptr);
		}
		else if (strcmp(command, "") != 0)
		{
			printf("Unknown command: '%s'\n", command);
			printf("Type 'help' to see help\n");
		}
	}
}

/////////////////////////////////////////////////////////////////////
static void Start(const std::vector<MountPoint>& paths, unsigned int uid,
	unsigned int gid, bool enableLog)
{
	DatagramSocket DatagramSockets[SOCKET_NUM];
	ServerSocket ServerSockets[SOCKET_NUM];

	auto rpcServer = std::make_unique<RPCServer>();
	auto portMapper = std::make_unique<PortmapProg>();
	auto nfsServer = std::make_unique<NFSProg>();
	auto mountServer = std::make_unique<MountProg>();

	portMapper->Set(PROG_MOUNT, MOUNT_PORT);  //map port for mount
	portMapper->Set(PROG_NFS, NFS_PORT);  //map port for nfs
	nfsServer->SetUserID(uid, gid);  //set uid and gid of files

	MountPaths(*mountServer.get(), paths);

	rpcServer->Set(PROG_PORTMAP, std::move(portMapper));  //program for portmap
	rpcServer->Set(PROG_NFS, std::move(nfsServer));       //program for nfs
	rpcServer->Set(PROG_MOUNT, std::move(mountServer));   //program for mount
	rpcServer->EnableLog(enableLog);

	for (int i = 0; i < SOCKET_NUM; i++)
	{
		DatagramSockets[i].SetListener(rpcServer.get());
		ServerSockets[i].SetListener(rpcServer.get());
	}

	bool success = false;

	if (ServerSockets[0].Open(PORTMAP_PORT, 3) && DatagramSockets[0].Open(PORTMAP_PORT)) //start portmap daemon
	{
		std::cout << "Portmap daemon started\n";
		if (ServerSockets[1].Open(NFS_PORT, 10) && DatagramSockets[1].Open(NFS_PORT)) //start nfs daemon
		{
			std::cout << "NFS daemon started\n";

			if (ServerSockets[2].Open(MOUNT_PORT, 3) && DatagramSockets[2].Open(MOUNT_PORT)) //start mount daemon
			{
				std::cout << "Mount daemon started\n";
				success = true;  //all daemon started
			}
			else
			{
				std::cout << "Mount daemon starts failed (check if port " << MOUNT_PORT << " is not already in use\n";
			}
		}
		else
		{
			std::cout << "NFS daemon starts failed.\n";
		}
	}
	else
	{
		std::cout << "Portmap daemon starts failed.\n";
	}

	if (success)
	{
		printf("Listening on %s\n", g_sInAddr);  //local address
		InputCommand(rpcServer.get());  //wait for commands
	}

	for (int i = 0; i < SOCKET_NUM; i++)
	{
		DatagramSockets[i].Close();
		ServerSockets[i].Close();
	}
}

/////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
	std::vector<MountPoint> paths;
	std::string pathFile;
	unsigned int uid = 0, gid = 0;
	bool enableLog = false;

	PrintAbout();

	if (argc < 2)
	{
		auto binaryName = strrchr(argv[0], '\\');
		binaryName = (binaryName == nullptr) ? argv[0] : binaryName + 1;
		PrintUsage(binaryName);
		return EXIT_FAILURE;
	}
	g_sInAddr = "0.0.0.0";

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-id") == 0)
		{
			uid = atoi(argv[++i]);
			gid = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-log") == 0)
		{
			enableLog = strcmp(argv[++i], "off") != 0;
		}
		else if (strcmp(argv[i], "-addr") == 0)
		{
			g_sInAddr = argv[++i];
		}
		else if (strcmp(argv[i], "-pathFile") == 0)
		{
			pathFile = argv[++i];
		}
		else if (i == argc - 2)
		{
			auto path = argv[argc - 2];  //path is before the last parameter
			auto alias = argv[argc - 1]; //path alias is the last parameter
			paths.emplace_back(path, alias);
			break;
		}
		else if (i == argc - 1)
		{
			auto path = argv[argc - 1];  //path is the last parameter
			paths.emplace_back(path, path);
			break;
		}
	}

	HWND console = GetConsoleWindow();
	if (enableLog == false && IsWindow(console))
	{
		ShowWindow(console, SW_HIDE); // hides the window
	}

	if (paths.empty() && pathFile.empty())
	{
		printf("No paths to mount\n");
		return EXIT_FAILURE;
	}

	WSADATA wsaData;
	WSAStartup(0x0101, &wsaData);
	Start(paths, uid, gid, enableLog);
	WSACleanup();

	return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}