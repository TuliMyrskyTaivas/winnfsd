/////////////////////////////////////////////////////////////////////
/// file: MountProg.cpp
///
/// summary: NFS mount RPC
/////////////////////////////////////////////////////////////////////

#include "MountProg.h"
#include "FileTable.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <cstring>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <direct.h>

enum
{
    MOUNTPROC_NULL = 0,
    MOUNTPROC_MNT = 1,
    MOUNTPROC_DUMP = 2,
    MOUNTPROC_UMNT = 3,
    MOUNTPROC_UMNTALL = 4,
    MOUNTPROC_EXPORT = 5
};

enum
{
    MNT_OK = 0,
    MNTERR_PERM = 1,
    MNTERR_NOENT = 2,
    MNTERR_IO = 5,
    MNTERR_ACCESS = 13,
    MNTERR_NOTDIR = 20,
    MNTERR_INVAL = 22
};

typedef int (MountProg::*PPROC)(void);

/////////////////////////////////////////////////////////////////////
MountProg::MountProg()
	: RPCProg()
	, m_mountNum(0)
	, m_pathFile(nullptr)
	, m_inStream(nullptr)
	, m_outStream(nullptr)
	, m_param(nullptr)
{
    memset(m_clientAddr, 0, sizeof(m_clientAddr));
}

/////////////////////////////////////////////////////////////////////
MountProg::~MountProg()
{
    if (m_pathFile)
	{
        free(m_pathFile);
    }	

    for (int i = 0; i < MOUNT_NUM_MAX; i++)
	{
        delete[] m_clientAddr[i];
    }

}

/////////////////////////////////////////////////////////////////////
bool MountProg::SetPathFile(const char *file)
{
	char *formattedFile = FormatPath(file, FORMAT_PATH);

	if (!formattedFile)
	{
		return false;
	}

	std::ifstream pathFile(formattedFile);

	if (pathFile.good())
	{
		pathFile.close();
		if (m_pathFile)
		{
			free(m_pathFile);
		}
		m_pathFile = formattedFile;
		return true;
	}

	pathFile.close();
	free(formattedFile);
	return false;
}

/////////////////////////////////////////////////////////////////////
void MountProg::Export(const char *path, const char *pathAlias)
{
	char *formattedPath = FormatPath(path, FORMAT_PATH);
	pathAlias = FormatPath(pathAlias, FORMAT_PATHALIAS);

	if (path != nullptr && pathAlias != nullptr)
	{
		if (m_pathMap.count(pathAlias) == 0)
		{
			m_pathMap[pathAlias] = formattedPath;
			printf("Path #%lld is: %s, path alias is: %s\n", m_pathMap.size(), path, pathAlias);
		}
		else
		{
			printf("Path %s with path alias %s already known\n", path, pathAlias);
		}

		free(formattedPath);
	}

}

/////////////////////////////////////////////////////////////////////
bool MountProg::Refresh()
{
	if (m_pathFile != nullptr)
	{
		ReadPathsFromFile(m_pathFile);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
int MountProg::GetMountNumber() const noexcept
{
    return m_mountNum;  //the number of clients mounted
}

/////////////////////////////////////////////////////////////////////
char* MountProg::GetClientAddr(int index)
{
    if (index < 0 || index >= m_mountNum)
	{
        return nullptr;
    }

    for (int i = 0; i < MOUNT_NUM_MAX; i++)
	{
        if (m_clientAddr[i] != nullptr)
		{
            if (index == 0)
			{
                return m_clientAddr[i];  //client address
            }
			else
			{
                --index;
            }
        }

    }
    return nullptr;
}

/////////////////////////////////////////////////////////////////////
int MountProg::Process(IInputStream *pInStream, IOutputStream *pOutStream, ProcessParam *pParam)
{
    static PPROC pf[] = { &MountProg::ProcedureNULL, &MountProg::ProcedureMNT, &MountProg::ProcedureNOIMP, &MountProg::ProcedureUMNT, &MountProg::ProcedureUMNTALL, &MountProg::ProcedureEXPORT };

    PrintLog("MOUNT ");

    if (pParam->nProc >= sizeof(pf) / sizeof(PPROC))
	{
        ProcedureNOIMP();
        PrintLog("\n");
        return PRC_NOTIMP;
    }

    m_inStream = pInStream;
    m_outStream = pOutStream;
    m_param = pParam;
    return (this->*pf[pParam->nProc])();
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureNULL() noexcept
{
    PrintLog("NULL");
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureMNT() noexcept
{
	Refresh();
    char *path = new char[MAXPATHLEN + 1];

	PrintLog("MNT");
	PrintLog(" from %s\n", m_param->pRemoteAddr);

	if (GetPath(&path))
	{
		m_outStream->Write(MNT_OK); //OK

		if (m_param->nVersion == 1)
		{
			m_outStream->Write(GetFileHandle(path), FHSIZE);  //fhandle
		}
		else
		{
			m_outStream->Write(NFS3_FHSIZE);  //length
			m_outStream->Write(GetFileHandle(path), NFS3_FHSIZE);  //fhandle
			m_outStream->Write(0);  //flavor
		}

		++m_mountNum;

		for (int i = 0; i < MOUNT_NUM_MAX; i++)
		{
			if (m_clientAddr[i] == nullptr) //search an empty space
			{ 
				m_clientAddr[i] = new char[strlen(m_param->pRemoteAddr) + 1];
				strcpy_s(m_clientAddr[i], (strlen(m_param->pRemoteAddr) + 1), m_param->pRemoteAddr);  //remember the client address
				break;
			}
		}
	}
	else
	{
		m_outStream->Write(MNTERR_ACCESS);  //permission denied
    }
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureUMNT() noexcept
{
	char *path = new char[MAXPATHLEN + 1];

    PrintLog("UMNT");
    GetPath(&path);
    PrintLog(" from %s", m_param->pRemoteAddr);

    for (int i = 0; i < MOUNT_NUM_MAX; i++) {
        if (m_clientAddr[i] != NULL) {
            if (strcmp(m_param->pRemoteAddr, m_clientAddr[i]) == 0) { //address match
                delete[] m_clientAddr[i];  //remove this address
                m_clientAddr[i] = NULL;
                --m_mountNum;
                break;
            }
        }
    }

	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureEXPORT() noexcept
{
	PrintLog("EXPORT");

	for (auto const &exportedPath : m_pathMap)
	{
		const char* path = exportedPath.first.c_str();
		int length = static_cast<int>(strlen(path));
		// dirpath
		m_outStream->Write(1);
		m_outStream->Write(length);
		m_outStream->Write(const_cast<char*>(path), length);
		int fillBytes = (length % 4);
		if (fillBytes > 0)
		{
			fillBytes = 4 - fillBytes;
			m_outStream->Write(".", fillBytes);
		}
		// groups
		m_outStream->Write(1);
		m_outStream->Write(1);
		m_outStream->Write("*", 1);
		m_outStream->Write("...", 3);
		m_outStream->Write(0);
	}

	m_outStream->Write(0);
	m_outStream->Write(0);
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureUMNTALL() noexcept
{
	PrintLog("UMNTALL NOIMP");
	return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureNOIMP() noexcept
{
    PrintLog("NOIMP");
    return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
bool MountProg::GetPath(char **returnPath)
{
	unsigned long i, size;
	static char path[MAXPATHLEN + 1];
	static char finalPath[MAXPATHLEN + 1];
	bool foundPath = false;

	m_inStream->Read(&size);

	if (size > MAXPATHLEN)
	{
		size = MAXPATHLEN;
	}

	typedef std::map<std::string, std::string>::iterator it_type;
	m_inStream->Read(path, size);
	path[size] = '\0';

	// TODO: this whole method is quite ugly and ripe for refactoring
	// strip slashes
	std::string pathTemp(path);
	pathTemp.erase(pathTemp.find_last_not_of("/\\") + 1);
	std::copy(pathTemp.begin(), pathTemp.end(), path);
	path[pathTemp.size()] = '\0';

	for (it_type iterator = m_pathMap.begin(); iterator != m_pathMap.end(); iterator++)
	{

		// strip slashes
		std::string pathAliasTemp(iterator->first.c_str());
		pathAliasTemp.erase(pathAliasTemp.find_last_not_of("/\\") + 1);
		char* pathAlias = const_cast<char*>(pathAliasTemp.c_str());

		// strip slashes
		std::string windowsPathTemp(iterator->second.c_str());
		// if it is a drive letter, e.g. D:\ keep the slash
		if (windowsPathTemp.substr(windowsPathTemp.size() - 2) != ":\\")
		{
			windowsPathTemp.erase(windowsPathTemp.find_last_not_of("/\\") + 1);
		}
		char* windowsPath = const_cast<char*>(windowsPathTemp.c_str());

		size_t aliasPathSize = strlen(pathAlias);
		size_t windowsPathSize = strlen(windowsPath);
		size_t requestedPathSize = pathTemp.size();

		if ((requestedPathSize > aliasPathSize) && (strncmp(path, pathAlias, aliasPathSize) == 0))
		{
			foundPath = true;
			//The requested path starts with the alias. Let's replace the alias with the real path
			strncpy_s(finalPath, MAXPATHLEN, windowsPath, windowsPathSize);
			strncpy_s(finalPath + windowsPathSize, MAXPATHLEN - windowsPathSize, (path + aliasPathSize), requestedPathSize - aliasPathSize);
			finalPath[windowsPathSize + requestedPathSize - aliasPathSize] = '\0';

			for (i = 0; i < requestedPathSize - aliasPathSize; i++)
			{
				//transform path to Windows format
				if (finalPath[windowsPathSize + i] == '/') {
					finalPath[windowsPathSize + i] = '\\';
				}
			}
		}
		else if ((requestedPathSize == aliasPathSize) && (strncmp(path, pathAlias, aliasPathSize) == 0))
		{
			foundPath = true;
			//The requested path IS the alias
			strncpy_s(finalPath, MAXPATHLEN, windowsPath, windowsPathSize);
			finalPath[windowsPathSize] = '\0';
		}

		if (foundPath == true)
		{
			break;
		}
	}

	if (foundPath != true)
	{
		//The requested path does not start with the alias, let's treat it normally.
		strncpy_s(finalPath, MAXPATHLEN, path, size);
		//transform mount path to Windows format. /d/work => d:\work
		finalPath[0] = finalPath[1];
		finalPath[1] = ':';

		for (i = 2; i < size; i++)
		{
			if (finalPath[i] == '/')
			{
				finalPath[i] = '\\';
			}
		}

		finalPath[size] = '\0';
	}

	PrintLog("Final local requested path: %s\n", finalPath);

	if ((size & 3) != 0)
	{
		m_inStream->Read(&i, 4 - (size & 3));  //skip opaque bytes
	}

	*returnPath = finalPath;
	return foundPath;
}

/////////////////////////////////////////////////////////////////////
bool MountProg::ReadPathsFromFile(const char* fileName)
{
	std::ifstream pathFile(fileName);

	if (pathFile.is_open())
	{
		std::string line, path;
		std::vector<std::string> paths;
		std::istringstream ss;

		while (std::getline(pathFile, line))
		{
			ss.clear();
			paths.clear();
			ss.str(line);

			// split path and alias separated by '>'
			while (std::getline(ss, path, '>'))
			{
				paths.push_back(path);
			}
			if (paths.size() < 1)
			{
				continue;
			}
			if (paths.size() < 2)
			{
				paths.push_back(paths[0]);
			}

			// clean path, trim spaces and slashes (except drive letter)
			paths[0].erase(paths[0].find_last_not_of(" ") + 1);
			if (paths[0].substr(paths[0].size() - 2) != ":\\")
			{
				paths[0].erase(paths[0].find_last_not_of("/\\ ") + 1);
			}

			char *pCurPath = (char*)malloc(paths[0].size() + 1);
			pCurPath = (char*)paths[0].c_str();
			
			if (pCurPath != NULL)
			{
				char *pCurPathAlias = (char*)malloc(paths[1].size() + 1);
				pCurPathAlias = (char*)paths[1].c_str();
				Export(pCurPath, pCurPathAlias);
			}
		}
	}
	else
	{
		printf("Can't open file %s.\n", fileName);
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
char *MountProg::FormatPath(const char *path, PathFormat format)
{
    size_t len = strlen(path);

	//Remove head spaces
	while (*path == ' ')
	{
		++path;
		len--;
	}

	//Remove tail spaces
	while (len > 0 && *(path + len - 1) == ' ')
	{
		len--;
	}

	//Remove windows tail slashes (except when its only a drive letter)
	while (len > 0 && *(path + len - 2) != ':' && *(path + len - 1) == '\\')
	{
		len--;
	}

	//Remove unix tail slashes
	while (len > 1 && *(path + len - 1) == '/')
	{
		len--;
	}

	//Is comment?
	if (*path == '#')
	{
		return nullptr;
	}

	//Remove head "
	if (*path == '"')
	{
		++path;
		len--;
	}

	//Remove tail "
	if (len > 0 && *(path + len - 1) == '"')
	{
		len--;
	}

	if (len < 1)
	{
		return nullptr;
	}

	char *result = (char *)malloc(len + 1);
	strncpy_s(result, len + 1, path, len);

	//Check for right path format
	if (format == FORMAT_PATH)
	{
		if (result[0] == '.')
		{
			static char path1[MAXPATHLEN];
			_getcwd(path1, MAXPATHLEN);

			if (result[1] == '\0')
			{
				len = strlen(path1);
				result = (char *)realloc(result, len + 1);
				strcpy_s(result, len + 1, path1);
			}
			else if (result[1] == '\\')
			{
				strcat_s(path1, result + 1);
				len = strlen(path1);
				result = (char *)realloc(result, len + 1);
				strcpy_s(result, len + 1, path1);
			}

		}
		if (len >= 2 && result[1] == ':' && ((result[0] >= 'A' && result[0] <= 'Z') || (result[0] >= 'a' && result[0] <= 'z'))) //check path format
		{
			char tempPath[MAXPATHLEN] = "\\\\?\\";
			strcat_s(tempPath, result);
			len = strlen(tempPath);
			result = (char *)realloc(result, len + 1);
			strcpy_s(result, len + 1, tempPath);
		}

		if (len < 6 || result[5] != ':' || !((result[4] >= 'A' && result[4] <= 'Z') || (result[4] >= 'a' && result[4] <= 'z'))) //check path format
		{
			printf("Path %s format is incorrect.\n", path);
			printf("Please use a full path such as C:\\work or \\\\?\\C:\\work\n");
			free(result);
			return NULL;
		}

		for (size_t i = 0; i < len; i++)
		{
			if (result[i] == '/')
			{
				result[i] = '\\';
			}
		}
	}
	else if (format == FORMAT_PATHALIAS)
	{
		if (path[1] == ':' && ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')))
		{
            strncpy_s(result, len + 1, path, len);
			//transform Windows format to mount path d:\work => /d/work
            result[1] = result[0];
            result[0] = '/';
			for (size_t i = 2; i < strlen(result); i++)
			{
				if (result[i] == '\\')
				{
                    result[i] = '/';
				}
			}
		}
		else if (path[0] != '/') //check path alias format
		{
			printf("Path alias format is incorrect.\n");
			printf("Please use a path like /exports\n");
			free(result);
			return nullptr;
		}
	}

	return result;
}
