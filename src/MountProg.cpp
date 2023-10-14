/////////////////////////////////////////////////////////////////////
/// file: MountProg.cpp
///
/// summary: NFS mount RPC
/////////////////////////////////////////////////////////////////////

#include "MountProg.h"
#include "FileTable.h"
#include "InputStream.h"
#include "OutputStream.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/log/trivial.hpp>

#include <cstring>
#include <cctype>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
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

/////////////////////////////////////////////////////////////////////
void MountProg::Export(const std::string& path, const std::string& alias)
{
	const auto formattedPath = FormatPath(path, FORMAT_PATH);
	const auto formattedAlias = FormatPath(alias, FORMAT_PATHALIAS);

	if (m_pathMap.count(alias))
	{
		throw std::runtime_error("Path " + alias + " already exported");
	}

	m_pathMap[alias] = formattedPath;
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: add export " << alias << '=' << formattedPath;
}

/////////////////////////////////////////////////////////////////////
int MountProg::Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	typedef int (MountProg::* PPROC)(IInputStream&, IOutputStream&, RPCParam&);
	static PPROC pf[] = { &MountProg::ProcedureNULL, &MountProg::ProcedureMNT, &MountProg::ProcedureNOIMP,
		&MountProg::ProcedureUMNT, &MountProg::ProcedureUMNTALL, &MountProg::ProcedureEXPORT };

	if (param.procNum >= sizeof(pf) / sizeof(PPROC))
	{
		BOOST_LOG_TRIVIAL(debug) << "MOUNT: procedure " << param.procNum << " not implemented";
		return PRC_NOTIMP;
	}

	return (this->*pf[param.procNum])(inStream, outStream, param);
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureNULL(IInputStream&, IOutputStream&, RPCParam&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: NULL command";
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureMNT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: MNT command, version=" << param.version << ", from " << param.remoteAddr;
	const auto path = GetPath(inStream);
	if (path.empty())
	{
		BOOST_LOG_TRIVIAL(debug) << "MOUNT: permission denied";
		outStream.Write(MNTERR_ACCESS);  //permission denied
		return PRC_OK;
	}

	const auto handle = GetFileHandle(path.c_str());
	outStream.Write(MNT_OK); //OK
	if (param.version == 1)
	{
		outStream.Write(handle, FHSIZE);  //fhandle
	}
	else
	{
		outStream.Write(NFS3_FHSIZE);  //length
		outStream.Write(handle, NFS3_FHSIZE);  //fhandle
		outStream.Write(0);  //flavor
	}

	m_clients.push_back(param.remoteAddr); //remember the client address
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureUMNT(IInputStream& inStream, IOutputStream&, RPCParam& param) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: MNT command, version=" << param.version << ", from " << param.remoteAddr;
	const auto path = GetPath(inStream);

	auto client = std::find(m_clients.begin(), m_clients.end(), param.remoteAddr);
	if (m_clients.end() != client)
	{
		m_clients.erase(client);
	}
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureEXPORT(IInputStream&, IOutputStream& outStream, RPCParam& param) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: EXPORT command, version=" << param.version << ", from " << param.remoteAddr;

	for (auto const& exportedPath : m_pathMap)
	{
		const auto& path = exportedPath.first;
		const uint32_t pathSize = static_cast<uint32_t>(path.length());
		// dirpath
		outStream.Write(1);
		outStream.Write(pathSize);
		outStream.Write(const_cast<char*>(path.data()), pathSize);
		uint32_t fillBytes = (pathSize % 4);
		if (fillBytes > 0)
		{
			fillBytes = 4 - fillBytes;
			outStream.Write(".", fillBytes);
		}
		// groups
		outStream.Write(1);
		outStream.Write(1);
		outStream.Write("*", 1);
		outStream.Write("...", 3);
		outStream.Write(0);
	}

	outStream.Write(0);
	outStream.Write(0);
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureUMNTALL(IInputStream&, IOutputStream&, RPCParam&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: UMNTALL command (not implemented)";
	return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int MountProg::ProcedureNOIMP(IInputStream&, IOutputStream&, RPCParam&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: NOIMP command (not implemented)";
	return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
std::string MountProg::GetPath(IInputStream& inStream)
{
	uint32_t i;
	static char finalPath[MAXPATHLEN + 1];
	bool foundPath = false;

	uint32_t pathSize = 0;
	inStream.Read(&pathSize);

	if (pathSize > MAXPATHLEN)
	{
		pathSize = MAXPATHLEN;
	}

	using it_type = std::map<std::string, std::string>::iterator;
	char path[MAXPATHLEN + 1];
	inStream.Read(path, pathSize);
	path[pathSize] = '\0';

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
		strncpy_s(finalPath, MAXPATHLEN, path, pathSize);
		//transform mount path to Windows format. /d/work => d:\work
		finalPath[0] = finalPath[1];
		finalPath[1] = ':';

		for (i = 2; i < pathSize; i++)
		{
			if (finalPath[i] == '/')
			{
				finalPath[i] = '\\';
			}
		}

		finalPath[pathSize] = '\0';
	}

	BOOST_LOG_TRIVIAL(debug) << "MOUNT: local requested path: " << finalPath;

	// skip opaque bytes
	if ((pathSize & 3) != 0)
	{
		inStream.Skip(4 - (pathSize & 3));
	}

	return finalPath;
}

/////////////////////////////////////////////////////////////////////
std::string MountProg::FormatPath(const std::string& path, PathFormat format) const
{
	std::string result = boost::algorithm::trim_copy(path);
	if (result.empty())
	{
		throw std::runtime_error("invalid path");
	}

	// Remove tail windows/unix slash
	if ('\\' == result.back() || '/' == result.back())
	{
		result.pop_back();
	}

	// Remove head and tail quotes
	boost::algorithm::trim_if(result, boost::is_any_of("\""));
	if (result.empty())
	{
		throw std::runtime_error("invalid path");
	}

	// Check for right path format
	if (format == FORMAT_PATH)
	{
		if (1 == result.length())
		{
			if ('.' == result.front())
			{
				result = std::filesystem::current_path().string();
			}
		}
		else if (result[1] == ':' && isalpha(result[0])) // check if path starts with a drive letter
		{
			result = "\\\\?\\" + result;
		}

		// replace slashes to backslashes
		for (auto& byte : result)
		{
			if ('/' == byte)
			{
				byte = '\\';
			}
		}
	}
	else if (format == FORMAT_PATHALIAS)
	{
		if (path[1] == ':' && isalpha(path[0]))
		{
			//transform Windows format to mount path d:\work => /d/work
			result[1] = result[0];
			result[0] = '/';
			for (size_t i = 2; i < result.size(); i++)
			{
				if (result[i] == '\\')
				{
					result[i] = '/';
				}
			}
		}
		else if (path[0] != '/') //check path alias format
		{
			throw std::runtime_error("Path alias format is incorrect. Please use a path like /exports");
		}
	}

	return result;
}