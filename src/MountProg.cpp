/////////////////////////////////////////////////////////////////////
/// file: MountProg.cpp
///
/// summary: NFS mount RPC
/////////////////////////////////////////////////////////////////////

#include "MountProg.h"
#include "FileTable.h"
#include "InputStream.h"
#include "OutputStream.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
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
	BOOST_LOG_TRIVIAL(debug) << "MOUNT: UMNT command, version=" << param.version << ", from " << param.remoteAddr;
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
	bool foundPath = false;
	std::string finalPath;

	// Read size of path
	uint32_t pathSize = 0;
	inStream.Read(&pathSize);
	if (pathSize > MAXPATHLEN)
	{
		pathSize = MAXPATHLEN;
	}

	// Read path
	char pathBuf[MAXPATHLEN + 1];
	inStream.Read(pathBuf, pathSize);
	pathBuf[pathSize] = '\0';

	// strip slashes
	std::string path(pathBuf);
	boost::algorithm::trim_left_if(path, boost::is_any_of("/\\"));

	for (const auto& mappedPath : m_pathMap)
	{
		// strip slashes from alias
		std::string alias = boost::trim_left_copy_if(mappedPath.first, boost::is_any_of("/\\"));

		// strip slashes from read windows patj
		std::string windowsPath = mappedPath.second;
		// if it is a drive letter, e.g. D:\ keep the slash
		if (windowsPath.substr(windowsPath.size() - 2) != ":\\")
		{
			windowsPath.erase(windowsPath.find_last_not_of("/\\") + 1);
		}

		if (boost::algorithm::starts_with(path, alias))
		{
			foundPath = true;
			//The requested path starts with the alias. Let's replace the alias with the real path
			finalPath = windowsPath + path.substr(alias.size());
			boost::algorithm::replace_all(finalPath, "/", "\\");
			break;
		}
		else if (path == alias)
		{
			//The requested path IS the alias
			foundPath = true;
			finalPath = windowsPath;
			break;
		}
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
		if (result[1] == ':' && isalpha(result[0]))
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
		else if (result[0] != '/') //check path alias format
		{
			throw std::runtime_error("Path alias format is incorrect. Please use a path like /exports");
		}
	}

	return result;
}