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

static unsigned int g_nUID, g_nGID;
static bool g_bLogOn;
static char *g_sFileName;
static RPCServer g_RPCServer;
static PortmapProg g_PortmapProg;
static NFSProg g_NFSProg;
static MountProg g_MountProg;

struct MountPoint
{
    std::string path;
    std::string alias;

    MountPoint(const char* path, const char* alias)
        : path(path)
        , alias(alias)
    {}
};

/////////////////////////////////////////////////////////////////////
static void PrintUsage(char *exeFilename)
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
static void PrintCount()
{
    std::cout << "There are " << g_MountProg.GetMountNumber() << " clients mounted\n";
}

/////////////////////////////////////////////////////////////////////
static void PrintMounts()
{
    PrintLine();
    const int mounts = g_MountProg.GetMountNumber();

    for (int i = 0; i < mounts; i++)
    {
        std::cout << g_MountProg.GetClientAddr(i);
    }

    PrintCount();
    PrintLine();
}

/////////////////////////////////////////////////////////////////////
static void PrintConfirmQuit()
{
    printf("\n");
    PrintCount();
    printf("Are you sure to quit? (y/N): ");
}

/////////////////////////////////////////////////////////////////////
static void MountPaths(const std::vector<MountPoint>& paths)
{
	for (const auto& mountPoint : paths)
    {
		g_MountProg.Export(mountPoint.path.c_str(), mountPoint.alias.c_str());
	}
}

/////////////////////////////////////////////////////////////////////
static void InputCommand()
{
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
			g_RPCServer.EnableLog(true);
		}
        else if (_stricmp(command, "log off") == 0)
        {
			g_RPCServer.EnableLog(false);
		}
        else if (_stricmp(command, "list") == 0)
        {
			PrintMounts();
		}
        else if (_stricmp(command, "quit") == 0)
        {
			if (g_MountProg.GetMountNumber() == 0)
            {
				break;
			}
            else
            {
				PrintConfirmQuit();
				fgets(command, 20, stdin);

				if (command[0] == 'y' || command[0] == 'Y')
                {
					break;
				}
			}
		}
        else if (_stricmp(command, "refresh") == 0)
        {
			g_MountProg.Refresh();
		}
        else if (_stricmp(command, "reset") == 0)
        {
			g_RPCServer.Set(PROG_NFS, nullptr);
		}
        else if (strcmp(command, "") != 0)
        {
			printf("Unknown command: '%s'\n", command);
			printf("Type 'help' to see help\n");
		}
	}
}

/////////////////////////////////////////////////////////////////////
static void Start(const std::vector<MountPoint>& paths)
{
	DatagramSocket DatagramSockets[SOCKET_NUM];
	ServerSocket ServerSockets[SOCKET_NUM];

	g_PortmapProg.Set(PROG_MOUNT, MOUNT_PORT);  //map port for mount
	g_PortmapProg.Set(PROG_NFS, NFS_PORT);  //map port for nfs
	g_NFSProg.SetUserID(g_nUID, g_nGID);  //set uid and gid of files

	MountPaths(paths);

	g_RPCServer.Set(PROG_PORTMAP, &g_PortmapProg);  //program for portmap
	g_RPCServer.Set(PROG_NFS, &g_NFSProg);  //program for nfs
	g_RPCServer.Set(PROG_MOUNT, &g_MountProg);  //program for mount
	g_RPCServer.EnableLog(g_bLogOn);

	for (int i = 0; i < SOCKET_NUM; i++)
    {
		DatagramSockets[i].SetListener(&g_RPCServer);
		ServerSockets[i].SetListener(&g_RPCServer);
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
		InputCommand();  //wait for commands
	}

	for (int i = 0; i < SOCKET_NUM; i++)
    {
		DatagramSockets[i].Close();
		ServerSockets[i].Close();
	}
}

/////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
try
{
    std::vector<MountPoint> paths;
    char *pathPointer = nullptr;
	bool pathFile = false;

    PrintAbout();

    if (argc < 2)
    {
        pathPointer = strrchr(argv[0], '\\');
        pathPointer = pathPointer == nullptr ? argv[0] : pathPointer + 1;
        PrintUsage(pathPointer);
        return 1;
    }

    g_nUID = g_nGID = 0;
    g_bLogOn = true;
    g_sFileName = nullptr;
	g_sInAddr = "0.0.0.0";

    for (int i = 1; i < argc; i++)
    {
        if (_stricmp(argv[i], "-id") == 0)
        {
            g_nUID = atoi(argv[++i]);
            g_nGID = atoi(argv[++i]);
        }
        else if (_stricmp(argv[i], "-log") == 0)
        {
            g_bLogOn = _stricmp(argv[++i], "off") != 0;
        }
        else if (_stricmp(argv[i], "-addr") == 0)
        {
			g_sInAddr = argv[++i];
		}
        else if (_stricmp(argv[i], "-pathFile") == 0)
        {
            g_sFileName = argv[++i];

			if (g_MountProg.SetPathFile(g_sFileName) == false)
            {
                printf("Can't open file %s.\n", g_sFileName);
                return 1;
			}
            else
            {
				g_MountProg.Refresh();
				pathFile = true;
			}
        }
        else if (i == argc - 2)
        {
            pathPointer = argv[argc - 2];  //path is before the last parameter
            char* pathAlias = argv[argc - 1]; //path alias is the last parameter

            if (pathPointer != nullptr || pathAlias != nullptr)
            {
                paths.emplace_back(pathPointer, pathAlias);
            }

            break;
        }
        else if (i == argc - 1)
        {
            pathPointer = argv[argc - 1];  //path is the last parameter

            if (pathPointer != nullptr)
            {
                char curPathAlias[MAXPATHLEN];
                strcpy_s(curPathAlias, pathPointer);
                char *pCurPathAlias = curPathAlias;

                paths.emplace_back(pathPointer, pCurPathAlias);
            }

            break;
        }
    }

    HWND console = GetConsoleWindow();

    if (g_bLogOn == false && IsWindow(console))
    {
        ShowWindow(console, SW_HIDE); // hides the window
    }

	if (paths.size() <= 0 && !pathFile)
    {
        printf("No paths to mount\n");
        return 1;
    }

    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
    Start(paths);
    WSACleanup();

    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}