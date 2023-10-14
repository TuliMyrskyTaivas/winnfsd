/////////////////////////////////////////////////////////////////////
/// file: NFS3Prog.h
///
/// summary: NFSv3 RPC
/////////////////////////////////////////////////////////////////////

#pragma comment(lib, "Shlwapi.lib")
#include "NFS3Prog.h"
#include "FileTable.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <string.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include <assert.h>
#include <string>
#include <windows.h>
#include <time.h>
#include <share.h>
#include <shlwapi.h>

#include <boost/log/trivial.hpp>
#define BUFFER_SIZE 1000

/////////////////////////////////////////////////////////////////////
struct Opaque
{
	uint32_t length;
	unsigned char* contents;

	Opaque();
	Opaque(uint32_t len);
	virtual ~Opaque();
	virtual void SetSize(uint32_t len);
};

/////////////////////////////////////////////////////////////////////
struct NFSv3FileHandle : public Opaque
{
	NFSv3FileHandle();
	~NFSv3FileHandle() = default;
};

/////////////////////////////////////////////////////////////////////
struct NFSv3Filename : public Opaque
{
	char* name;

	NFSv3Filename();
	~NFSv3Filename() = default;
	void SetSize(uint32_t len);
	void Set(char* str);
};

/////////////////////////////////////////////////////////////////////
struct NFSv3Path : public Opaque
{
	char* path;

	NFSv3Path();
	~NFSv3Path() = default;
	void SetSize(uint32_t len);
	void Set(char* str);
};

/////////////////////////////////////////////////////////////////////
struct SpecData3
{
	uint32_t specdata1;
	uint32_t specdata2;
};

/////////////////////////////////////////////////////////////////////
struct NFSTime3
{
	uint32_t seconds;
	uint32_t nseconds;
};

/////////////////////////////////////////////////////////////////////
struct SAttrGuard3
{
	bool check;
	NFSTime3 objCtime;
};

/////////////////////////////////////////////////////////////////////
struct FAttr3
{
	FType3 type;
	Mode3 mode;
	uint32_t nlink;
	Uid3 uid;
	Gid3 gid;
	Size3 size;
	Size3 used;
	SpecData3 rdev;
	uint64_t fsid;
	FileId3 fileid;
	NFSTime3 atime;
	NFSTime3 mtime;
	NFSTime3 ctime;
};

/////////////////////////////////////////////////////////////////////
struct PostOpAttr
{
	bool attributesFollow;
	FAttr3 attributes;
};

/////////////////////////////////////////////////////////////////////
struct WccAttr
{
	Size3 size;
	NFSTime3 mtime;
	NFSTime3 ctime;
};

/////////////////////////////////////////////////////////////////////
struct PreOpAttr
{
	bool attributesFollow;
	WccAttr attributes;
};

/////////////////////////////////////////////////////////////////////
struct WccData
{
	PreOpAttr before;
	PostOpAttr after;
};

/////////////////////////////////////////////////////////////////////
struct PostOpFH3
{
	bool handleFollows;
	NFSv3FileHandle handle;
};

/////////////////////////////////////////////////////////////////////
struct SetMode3
{
	bool setIt;
	Mode3 mode;
};

/////////////////////////////////////////////////////////////////////
struct SetUid3
{
	bool setIt;
	Uid3 uid;
};

/////////////////////////////////////////////////////////////////////
struct SetGid3
{
	bool setIt;
	Gid3 gid;
};

/////////////////////////////////////////////////////////////////////
struct SetSize3
{
	bool setIt;
	Size3 size;
};

/////////////////////////////////////////////////////////////////////
struct SetAtime
{
	TimeHow setIt;
	NFSTime3 atime;
};

/////////////////////////////////////////////////////////////////////
struct SetMtime
{
	TimeHow setIt;
	NFSTime3 mtime;
};

/////////////////////////////////////////////////////////////////////
struct SAttr3
{
	SetMode3 mode;
	SetUid3 uid;
	SetGid3 gid;
	SetSize3 size;
	SetAtime atime;
	SetMtime mtime;
};

/////////////////////////////////////////////////////////////////////
struct DirOpArgs3
{
	NFSv3FileHandle dir;
	NFSv3Filename name;
};

/////////////////////////////////////////////////////////////////////
struct CreateHow3
{
	CreateMode3 mode;
	SAttr3 objAttributes;
	CreateVerf3 verf;
};

/////////////////////////////////////////////////////////////////////
struct SymlinkData3
{
	SAttr3 symlinkAttributes;
	NFSv3Path symlinkData;
};

/////////////////////////////////////////////////////////////////////
typedef struct
{
	ULONG  ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;
	union
	{
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			ULONG  Flags;
			WCHAR  PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			WCHAR  PathBuffer[1];
		} MountPointReparseBuffer;
		struct
		{
			UCHAR DataBuffer[1];
		} GenericReparseBuffer;
	};
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;

/////////////////////////////////////////////////////////////////////
enum
{
	NFSPROC3_NULL = 0,
	NFSPROC3_GETATTR = 1,
	NFSPROC3_SETATTR = 2,
	NFSPROC3_LOOKUP = 3,
	NFSPROC3_ACCESS = 4,
	NFSPROC3_READLINK = 5,
	NFSPROC3_READ = 6,
	NFSPROC3_WRITE = 7,
	NFSPROC3_CREATE = 8,
	NFSPROC3_MKDIR = 9,
	NFSPROC3_SYMLINK = 10,
	NFSPROC3_MKNOD = 11,
	NFSPROC3_REMOVE = 12,
	NFSPROC3_RMDIR = 13,
	NFSPROC3_RENAME = 14,
	NFSPROC3_LINK = 15,
	NFSPROC3_READDIR = 16,
	NFSPROC3_READDIRPLUS = 17,
	NFSPROC3_FSSTAT = 18,
	NFSPROC3_FSINFO = 19,
	NFSPROC3_PATHCONF = 20,
	NFSPROC3_COMMIT = 21
};

/////////////////////////////////////////////////////////////////////
enum
{
	NFS3_OK = 0,
	NFS3ERR_PERM = 1,
	NFS3ERR_NOENT = 2,
	NFS3ERR_IO = 5,
	NFS3ERR_NXIO = 6,
	NFS3ERR_ACCES = 13,
	NFS3ERR_EXIST = 17,
	NFS3ERR_XDEV = 18,
	NFS3ERR_NODEV = 19,
	NFS3ERR_NOTDIR = 20,
	NFS3ERR_ISDIR = 21,
	NFS3ERR_INVAL = 22,
	NFS3ERR_FBIG = 27,
	NFS3ERR_NOSPC = 28,
	NFS3ERR_ROFS = 30,
	NFS3ERR_MLINK = 31,
	NFS3ERR_NAMETOOLONG = 63,
	NFS3ERR_NOTEMPTY = 66,
	NFS3ERR_DQUOT = 69,
	NFS3ERR_STALE = 70,
	NFS3ERR_REMOTE = 71,
	NFS3ERR_BADHANDLE = 10001,
	NFS3ERR_NOT_SYNC = 10002,
	NFS3ERR_BAD_COOKIE = 10003,
	NFS3ERR_NOTSUPP = 10004,
	NFS3ERR_TOOSMALL = 10005,
	NFS3ERR_SERVERFAULT = 10006,
	NFS3ERR_BADTYPE = 10007,
	NFS3ERR_JUKEBOX = 10008
};

/////////////////////////////////////////////////////////////////////
enum
{
	NF3REG = 1,
	NF3DIR = 2,
	NF3BLK = 3,
	NF3CHR = 4,
	NF3LNK = 5,
	NF3SOCK = 6,
	NF3FIFO = 7
};

/////////////////////////////////////////////////////////////////////
enum
{
	ACCESS3_READ = 0x0001,
	ACCESS3_LOOKUP = 0x0002,
	ACCESS3_MODIFY = 0x0004,
	ACCESS3_EXTEND = 0x0008,
	ACCESS3_DELETE = 0x0010,
	ACCESS3_EXECUTE = 0x0020
};

/////////////////////////////////////////////////////////////////////
enum
{
	FSF3_LINK = 0x0001,
	FSF3_SYMLINK = 0x0002,
	FSF3_HOMOGENEOUS = 0x0008,
	FSF3_CANSETTIME = 0x0010
};

/////////////////////////////////////////////////////////////////////
enum
{
	UNSTABLE = 0,
	DATA_SYNC = 1,
	FILE_SYNC = 2
};

/////////////////////////////////////////////////////////////////////
enum
{
	DONT_CHANGE = 0,
	SET_TO_SERVER_TIME = 1,
	SET_TO_CLIENT_TIME = 2
};

/////////////////////////////////////////////////////////////////////
enum
{
	UNCHECKED = 0,
	GUARDED = 1,
	EXCLUSIVE = 2
};

/////////////////////////////////////////////////////////////////////
Opaque::Opaque()
{
	length = 0;
	contents = NULL;
}

/////////////////////////////////////////////////////////////////////
Opaque::Opaque(uint32_t len)
{
	contents = NULL;
	SetSize(len);
}

/////////////////////////////////////////////////////////////////////
Opaque::~Opaque()
{
	delete[] contents;
}

/////////////////////////////////////////////////////////////////////
void Opaque::SetSize(uint32_t len)
{
	delete[] contents;
	length = len;
	contents = new unsigned char[length];
	memset(contents, 0, length);
}

/////////////////////////////////////////////////////////////////////
NFSv3FileHandle::NFSv3FileHandle()
	: Opaque(NFS3_FHSIZE)
{}

/////////////////////////////////////////////////////////////////////
NFSv3Filename::NFSv3Filename()
	: Opaque()
	, name(nullptr)
{}

/////////////////////////////////////////////////////////////////////
void NFSv3Filename::SetSize(uint32_t len)
{
	Opaque::SetSize(len + 1);
	length = len;
	name = (char*)contents;
}

/////////////////////////////////////////////////////////////////////
void NFSv3Filename::Set(char* str)
{
	SetSize(static_cast<uint32_t>(strlen(str)));
	strcpy_s(name, (strlen(str) + 1), str);
}

/////////////////////////////////////////////////////////////////////
NFSv3Path::NFSv3Path()
	: Opaque()
	, path(nullptr)
{}

/////////////////////////////////////////////////////////////////////
void NFSv3Path::SetSize(uint32_t len)
{
	Opaque::SetSize(len + 1);
	length = len;
	path = (char*)contents;
}

/////////////////////////////////////////////////////////////////////
void NFSv3Path::Set(char* str)
{
	SetSize(static_cast<uint32_t>(strlen(str)));
	strcpy_s(path, (strlen(str) + 1), str);
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, bool& value)
{
	uint32_t b = 0;

	if (inStream.Read(&b) < sizeof(uint32_t))
	{
		throw std::runtime_error("read failed");
	}

	value = (b == 1);
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, uint32_t& value)
{
	if (inStream.Read(&value) < sizeof(uint32_t))
	{
		throw std::runtime_error("read failed");
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, uint64_t& value)
{
	if (inStream.Read8(&value) < sizeof(uint64_t))
	{
		throw std::runtime_error("read failed");
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, NFSTime3& value)
{
	Read(inStream, value.seconds);
	Read(inStream, value.nseconds);
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, SAttr3& value)
{
	Read(inStream, value.mode.setIt);
	if (value.mode.setIt)
	{
		Read(inStream, value.mode.mode);
	}

	Read(inStream, value.uid.setIt);
	if (value.uid.setIt)
	{
		Read(inStream, value.uid.uid);
	}

	Read(inStream, value.gid.setIt);
	if (value.gid.setIt)
	{
		Read(inStream, value.gid.gid);
	}

	Read(inStream, value.size.setIt);
	if (value.size.setIt)
	{
		Read(inStream, value.size.size);
	}

	Read(inStream, value.atime.setIt);
	if (value.atime.setIt == SET_TO_CLIENT_TIME)
	{
		Read(inStream, value.atime.atime);
	}

	Read(inStream, value.mtime.setIt);
	if (value.mtime.setIt == SET_TO_CLIENT_TIME)
	{
		Read(inStream, value.mtime.mtime);
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, SAttrGuard3& value)
{
	Read(inStream, value.check);

	if (value.check)
	{
		Read(inStream, value.objCtime);
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, Opaque& value)
{
	uint32_t len = 0;

	Read(inStream, len);
	value.SetSize(len);

	if (inStream.Read(value.contents, len) < len)
	{
		throw std::runtime_error("read failed");
	}

	len = 4 - (len & 3);
	if (len != 4)
	{
		uint32_t byte = 0;
		if (inStream.Read(&byte, len) < len)
		{
			throw std::runtime_error("read failed");
		}
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, DirOpArgs3& value)
{
	Read(inStream, value.dir);
	Read(inStream, value.name);
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, CreateHow3& value)
{
	Read(inStream, value.mode);
	if (value.mode == UNCHECKED || value.mode == GUARDED)
	{
		Read(inStream, value.objAttributes);
	}
	else
	{
		Read(inStream, value.verf);
	}
}

/////////////////////////////////////////////////////////////////////
void Read(IInputStream& inStream, SymlinkData3& value)
{
	Read(inStream, value.symlinkAttributes);
	Read(inStream, value.symlinkData);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const bool value)
{
	const uint32_t toWrite = value ? 1 : 0;
	outStream.Write(toWrite);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const uint32_t value)
{
	outStream.Write(value);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const uint64_t value)
{
	outStream.Write8(value);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const SpecData3& value)
{
	Write(outStream, value.specdata1);
	Write(outStream, value.specdata2);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const NFSTime3& value)
{
	Write(outStream, value.seconds);
	Write(outStream, value.nseconds);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const FAttr3& value)
{
	Write(outStream, value.type);
	Write(outStream, value.mode);
	Write(outStream, value.nlink);
	Write(outStream, value.uid);
	Write(outStream, value.gid);
	Write(outStream, value.size);
	Write(outStream, value.used);
	Write(outStream, value.rdev);
	Write(outStream, value.fsid);
	Write(outStream, value.fileid);
	Write(outStream, value.atime);
	Write(outStream, value.mtime);
	Write(outStream, value.ctime);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const Opaque& value)
{
	Write(outStream, value.length);
	outStream.Write(value.contents, value.length);

	const uint32_t len = value.length & 3;
	if (len != 0)
	{
		const uint32_t byte = 0;
		outStream.Write(byte, 4 - len);
	}
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const WccAttr& value)
{
	Write(outStream, value.size);
	Write(outStream, value.mtime);
	Write(outStream, value.ctime);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const PreOpAttr& value)
{
	Write(outStream, value.attributesFollow);
	if (value.attributesFollow)
	{
		Write(outStream, value.attributes);
	}
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const PostOpAttr& value)
{
	Write(outStream, value.attributesFollow);
	if (value.attributesFollow)
	{
		Write(outStream, value.attributes);
	}
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const WccData& value)
{
	Write(outStream, value.before);
	Write(outStream, value.after);
}

/////////////////////////////////////////////////////////////////////
void Write(IOutputStream& outStream, const PostOpFH3& value)
{
	Write(outStream, value.handleFollows);
	if (value.handleFollows)
	{
		Write(outStream, value.handle);
	}
}

/////////////////////////////////////////////////////////////////////
inline const char* NfsStatToString(NfsStat3 stat)
{
	switch (stat)
	{
	case NFS3_OK:
		return "OK";
	case NFS3ERR_PERM:
		return "PERM";
	case NFS3ERR_NOENT:
		return "NOENT";
	case NFS3ERR_IO:
		return "IO";
	case NFS3ERR_NXIO:
		return "NXIO";
	case NFS3ERR_ACCES:
		return "ACCESS";
	case NFS3ERR_EXIST:
		return "EXIST";
	case NFS3ERR_XDEV:
		return "XDEV";
	case NFS3ERR_NODEV:
		return "NODEV";
	case NFS3ERR_NOTDIR:
		return "NOTDIR";
	case NFS3ERR_ISDIR:
		return "ISDIR";
	case NFS3ERR_INVAL:
		return "INVAL";
	case NFS3ERR_FBIG:
		return "FBIG";
	case NFS3ERR_NOSPC:
		return "NOSPC";
	case NFS3ERR_ROFS:
		return "ROFS";
	case NFS3ERR_MLINK:
		return "MLINK";
	case NFS3ERR_NAMETOOLONG:
		return "NAMETOOLONG";
	case NFS3ERR_NOTEMPTY:
		return "NOTEMPTY";
	case NFS3ERR_DQUOT:
		return "DQUOT";
	case NFS3ERR_STALE:
		return "STALE";
	case NFS3ERR_REMOTE:
		return "REMOTE";
	case NFS3ERR_BADHANDLE:
		return "BADHANDLE";
	case NFS3ERR_NOT_SYNC:
		return "NOT_SYNC";
	case NFS3ERR_BAD_COOKIE:
		return "BAD_COOKIE";
	case NFS3ERR_NOTSUPP:
		return "NOTSUPP";
	case NFS3ERR_TOOSMALL:
		return "TOOSMALL";
	case NFS3ERR_SERVERFAULT:
		return "SERVERFAULT";
	case NFS3ERR_BADTYPE:
		return "BADTYPE";
	case NFS3ERR_JUKEBOX:
		return "JUKEBOX";
	default:
		return "UNKNOWN";
	}
}

/////////////////////////////////////////////////////////////////////
NFS3Prog::NFS3Prog(unsigned int uid, unsigned int gid)
	: RPCProg()
	, m_uid(uid)
	, m_gid(gid)
{}

/////////////////////////////////////////////////////////////////////
int NFS3Prog::Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	typedef NfsStat3(NFS3Prog::* PPROC)(IInputStream&, IOutputStream&, RPCParam&);
	static PPROC pf[] = {
		&NFS3Prog::ProcedureNULL, &NFS3Prog::ProcedureGETATTR, &NFS3Prog::ProcedureSETATTR,
		&NFS3Prog::ProcedureLOOKUP, &NFS3Prog::ProcedureACCESS, &NFS3Prog::ProcedureREADLINK,
		&NFS3Prog::ProcedureREAD, &NFS3Prog::ProcedureWRITE, &NFS3Prog::ProcedureCREATE,
		&NFS3Prog::ProcedureMKDIR, &NFS3Prog::ProcedureSYMLINK, &NFS3Prog::ProcedureMKNOD,
		&NFS3Prog::ProcedureREMOVE, &NFS3Prog::ProcedureRMDIR, &NFS3Prog::ProcedureRENAME,
		&NFS3Prog::ProcedureLINK, &NFS3Prog::ProcedureREADDIR, &NFS3Prog::ProcedureREADDIRPLUS,
		&NFS3Prog::ProcedureFSSTAT, &NFS3Prog::ProcedureFSINFO, &NFS3Prog::ProcedurePATHCONF,
		&NFS3Prog::ProcedureCOMMIT
	};

	if (param.procNum >= sizeof(pf) / sizeof(PPROC))
	{
		BOOST_LOG_TRIVIAL(error) << "NFS3 client " << param.remoteAddr << " requested non existing procedure " << param.procNum;
		return PRC_NOTIMP;
	}

	try
	{
		const auto stat = (this->*pf[param.procNum])(inStream, outStream, param);
		BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " procedure " << param.procNum
			<< " completed with code " << NfsStatToString(stat);
	}
	catch (const std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << "NFS3 procedure failed: " << e.what();
		return PRC_FAIL;
	}

	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureNULL(IInputStream&, IOutputStream&, RPCParam&)
{
	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureGETATTR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	FAttr3 attributes{};
	NfsStat3 stat = NFS3_OK;
	std::string path{};

	try
	{
		path = GetPath(inStream);
		stat = CheckFile(path);

		if (stat == NFS3ERR_NOENT)
		{
			stat = NFS3ERR_STALE;
		}
		else if (stat == NFS3_OK)
		{
			if (!GetFileAttributesForNFS(path, &attributes))
			{
				stat = NFS3ERR_IO;
			}
		}
	}
	catch (const std::exception&)
	{
		stat = NFS3ERR_STALE;
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " GETATTR '" << path << "': " << NfsStatToString(stat);
	Write(outStream, stat);
	if (stat == NFS3_OK)
	{
		Write(outStream, attributes);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureSETATTR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	SAttr3 newAttributes;
	SAttrGuard3 guard;
	WccData objWcc;
	NfsStat3 stat;
	int mode;
	FILE* file;
	HANDLE fileHandle;
	FILETIME fileTime;
	SYSTEMTIME systemTime;

	const std::string path = GetPath(inStream);
	Read(inStream, newAttributes);
	Read(inStream, guard);
	stat = CheckFile(path);
	objWcc.before.attributesFollow = GetFileAttributesForNFS(path, &objWcc.before.attributes);

	if (stat == NFS3_OK)
	{
		if (newAttributes.mode.setIt)
		{
			mode = 0;

			if ((newAttributes.mode.mode & 0x100) != 0)
			{
				mode |= S_IREAD;
			}

			// Always set read and write permissions (deliberately implemented this way)
			// if ((new_attributes.mode.mode & 0x80) != 0) {
			mode |= S_IWRITE;
			// }

			// S_IEXEC is not availabile on windows
			// if ((new_attributes.mode.mode & 0x40) != 0) {
			//     nMode |= S_IEXEC;
			// }

			if (_chmod(path.c_str(), mode) != 0)
			{
				stat = NFS3ERR_INVAL;
			}
		}

		// deliberately not implemented because we cannot reflect uid/gid on windows (easily)
		if (newAttributes.uid.setIt) {}
		if (newAttributes.gid.setIt) {}

		// deliberately not implemented
		if (newAttributes.mtime.setIt == SET_TO_CLIENT_TIME) {}
		if (newAttributes.atime.setIt == SET_TO_CLIENT_TIME) {}

		if (newAttributes.mtime.setIt == SET_TO_SERVER_TIME || newAttributes.atime.setIt == SET_TO_SERVER_TIME)
		{
			fileHandle = CreateFile(path.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				GetSystemTime(&systemTime);
				SystemTimeToFileTime(&systemTime, &fileTime);
				if (newAttributes.mtime.setIt == SET_TO_SERVER_TIME)
				{
					SetFileTime(fileHandle, NULL, NULL, &fileTime);
				}
				if (newAttributes.atime.setIt == SET_TO_SERVER_TIME)
				{
					SetFileTime(fileHandle, NULL, &fileTime, NULL);
				}
			}
			CloseHandle(fileHandle);
		}

		if (newAttributes.size.setIt)
		{
			file = _fsopen(path.c_str(), "r+b", _SH_DENYWR);
			if (file != nullptr)
			{
				int filedes = _fileno(file);
				_chsize_s(filedes, newAttributes.size.size);
				fclose(file);
			}
		}
	}

	objWcc.after.attributesFollow = GetFileAttributesForNFS(path, &objWcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, objWcc);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " SETATTR '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureLOOKUP(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	NFSv3FileHandle object;
	PostOpAttr fileAttributes;
	PostOpAttr dirAttributes;
	NfsStat3 stat;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);

	std::string path = GetFullPath(dirName, fileName);
	stat = CheckFile(dirName, path);
	if (stat == NFS3_OK)
	{
		GetFileHandle(path, &object);
		fileAttributes.attributesFollow = GetFileAttributesForNFS(path, &fileAttributes.attributes);
	}

	dirAttributes.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dirAttributes.attributes);

	Write(outStream, stat);

	if (stat == NFS3_OK)
	{
		Write(outStream, object);
		Write(outStream, fileAttributes);
	}

	Write(outStream, dirAttributes);
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " LOOKUP '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureACCESS(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	uint32_t access;
	PostOpAttr objAttributes;
	NfsStat3 stat;

	const std::string path = GetPath(inStream);
	Read(inStream, access);
	stat = CheckFile(path);

	if (stat == NFS3ERR_NOENT)
	{
		stat = NFS3ERR_STALE;
	}

	objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);

	Write(outStream, stat);
	Write(outStream, objAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, access);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " ACCESS '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureREADLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	PostOpAttr symlinkAttributes;
	NFSv3Path data = NFSv3Path();

	//opaque data;
	NfsStat3 stat;

	HANDLE hFile;
	REPARSE_DATA_BUFFER* lpOutBuffer;
	lpOutBuffer = (REPARSE_DATA_BUFFER*)malloc(MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
	DWORD bytesReturned;

	std::string path = GetPath(inStream);
	stat = CheckFile(path);
	if (stat == NFS3_OK) {
		hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_REPARSE_POINT | FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			stat = NFS3ERR_IO;
		}
		else
		{
			lpOutBuffer = (REPARSE_DATA_BUFFER*)malloc(MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
			if (!lpOutBuffer) {
				stat = NFS3ERR_IO;
			}
			else {
				DeviceIoControl(hFile, FSCTL_GET_REPARSE_POINT, NULL, 0, lpOutBuffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bytesReturned, NULL);
				std::string finalSymlinkPath;
				if (lpOutBuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK || lpOutBuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT)
				{
					if (lpOutBuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK)
					{
						size_t plen = lpOutBuffer->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(WCHAR);
						WCHAR* szPrintName = new WCHAR[plen + 1];
						wcsncpy_s(szPrintName, plen + 1, &lpOutBuffer->SymbolicLinkReparseBuffer.PathBuffer[lpOutBuffer->SymbolicLinkReparseBuffer.PrintNameOffset / sizeof(WCHAR)], plen);
						szPrintName[plen] = 0;
						std::wstring wStringTemp(szPrintName);
						delete[] szPrintName;
#pragma warning(push)
#pragma warning(disable:4244)
						std::string cPrintName(wStringTemp.begin(), wStringTemp.end());
#pragma warning(pop)
						finalSymlinkPath.assign(cPrintName);
						// TODO: Revisit with cleaner solution
						if (!PathIsRelative(cPrintName.c_str()))
						{
							std::string strFromChar;
							strFromChar.append("\\\\?\\");
							strFromChar.append(cPrintName);
							char* target = _strdup(strFromChar.c_str());
							// remove last folder
							size_t lastFolderIndex = path.find_last_of('\\');
							if (lastFolderIndex != std::string::npos) {
								path = path.substr(0, lastFolderIndex);
							}
							char szOut[MAX_PATH] = "";
							PathRelativePathTo(szOut, path.c_str(), FILE_ATTRIBUTE_DIRECTORY, target, FILE_ATTRIBUTE_DIRECTORY);
							std::string symlinkPath(szOut);
							finalSymlinkPath.assign(symlinkPath);
						}
					}

					// TODO: Revisit with cleaner solution
					if (lpOutBuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT)
					{
						size_t slen = lpOutBuffer->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
						WCHAR* szSubName = new WCHAR[slen + 1];
						wcsncpy_s(szSubName, slen + 1, &lpOutBuffer->MountPointReparseBuffer.PathBuffer[lpOutBuffer->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR)], slen);
						szSubName[slen] = 0;
						std::wstring wStringTemp(szSubName);
						delete[] szSubName;
						std::string target(wStringTemp.begin(), wStringTemp.end());
						target.erase(0, 2);
						target.insert(0, 2, '\\');
						// remove last folder, see above
						size_t lastFolderIndex = path.find_last_of('\\');
						if (lastFolderIndex != std::string::npos) {
							path = path.substr(0, lastFolderIndex);
						}
						char szOut[MAX_PATH] = "";
						PathRelativePathTo(szOut, path.c_str(), FILE_ATTRIBUTE_DIRECTORY, target.c_str(), FILE_ATTRIBUTE_DIRECTORY);
						std::string symlinkPath = szOut;
						finalSymlinkPath.assign(symlinkPath);
					}

					// write path always with / separator, so windows created symlinks work too
					std::replace(finalSymlinkPath.begin(), finalSymlinkPath.end(), '\\', '/');
					char* result = _strdup(finalSymlinkPath.c_str());
					data.Set(result);
				}
				free(lpOutBuffer);
			}
		}
		CloseHandle(hFile);
	}

	symlinkAttributes.attributesFollow = GetFileAttributesForNFS(path, &symlinkAttributes.attributes);

	Write(outStream, stat);
	Write(outStream, symlinkAttributes);
	if (stat == NFS3_OK) {
		Write(outStream, data);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " READLINK '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureREAD(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	Offset3 offset = 0;
	Count3 count = 0;
	PostOpAttr fileAttributes{};
	bool eof = false;
	Opaque data{};
	NfsStat3 stat{};
	FILE* pFile = nullptr;

	const std::string path = GetPath(inStream);
	Read(inStream, offset);
	Read(inStream, count);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		data.SetSize(count);
		pFile = _fsopen(path.c_str(), "rb", _SH_DENYWR);

		if (pFile != NULL)
		{
			_fseeki64(pFile, offset, SEEK_SET);
			count = static_cast<Count3>(fread(data.contents, sizeof(char), count, pFile));
			eof = fgetc(pFile) == EOF;
			fclose(pFile);
		}
		else
		{
			char buffer[BUFFER_SIZE];
			errno_t errorNumber = errno;
			strerror_s(buffer, BUFFER_SIZE, errorNumber);

			if (errorNumber == 13)
			{
				stat = NFS3ERR_ACCES;
			}
			else
			{
				stat = NFS3ERR_IO;
			}
		}
	}

	fileAttributes.attributesFollow = GetFileAttributesForNFS(path, &fileAttributes.attributes);

	Write(outStream, stat);
	Write(outStream, fileAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, count);
		Write(outStream, eof);
		Write(outStream, data);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " READ '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureWRITE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	Offset3 offset = 0;
	Count3 count = 0;
	StableHow stable{};
	Opaque data{};
	WccData fileWcc{};
	WriteVerf3 verf{};
	NfsStat3 stat{};
	FILE* pFile = nullptr;

	const std::string path = GetPath(inStream);
	Read(inStream, offset);
	Read(inStream, count);
	Read(inStream, stable);
	Read(inStream, data);
	stat = CheckFile(path);

	fileWcc.before.attributesFollow = GetFileAttributesForNFS(path, &fileWcc.before.attributes);

	if (stat == NFS3_OK)
	{
		if (stable == UNSTABLE)
		{
			NFSv3FileHandle handle;
			GetFileHandle(path, &handle);
			int handleId = *(unsigned int*)handle.contents;

			if (unstableStorageFile.count(handleId) == 0)
			{
				pFile = _fsopen(path.c_str(), "r+b", _SH_DENYWR);
				if (pFile != NULL)
				{
					unstableStorageFile.insert(std::make_pair(handleId, pFile));
				}
			}
			else
			{
				pFile = unstableStorageFile[handleId];
			}

			if (pFile != NULL)
			{
				_fseeki64(pFile, offset, SEEK_SET);
				count = static_cast<Count3>(fwrite(data.contents, sizeof(char), data.length, pFile));
			}
			else
			{
				char buffer[BUFFER_SIZE];
				errno_t errorNumber = errno;
				strerror_s(buffer, BUFFER_SIZE, errorNumber);

				if (errorNumber == 13)
				{
					stat = NFS3ERR_ACCES;
				}
				else
				{
					stat = NFS3ERR_IO;
				}
			}
			// this should not be zero but a timestamp (process start time) instead
			verf = 0;
			// we can reuse this, because no physical write has happend
			fileWcc.after.attributesFollow = fileWcc.before.attributesFollow;
		}
		else
		{
			pFile = _fsopen(path.c_str(), "r+b", _SH_DENYWR);

			if (pFile != NULL)
			{
				_fseeki64(pFile, offset, SEEK_SET);
				count = static_cast<Count3>(fwrite(data.contents, sizeof(char), data.length, pFile));
				fclose(pFile);
			}
			else
			{
				char buffer[BUFFER_SIZE];
				errno_t errorNumber = errno;
				strerror_s(buffer, BUFFER_SIZE, errorNumber);

				if (errorNumber == 13)
				{
					stat = NFS3ERR_ACCES;
				}
				else
				{
					stat = NFS3ERR_IO;
				}
			}

			stable = FILE_SYNC;
			verf = 0;

			fileWcc.after.attributesFollow = GetFileAttributesForNFS(path, &fileWcc.after.attributes);
		}
	}

	Write(outStream, stat);
	Write(outStream, fileWcc);

	if (stat == NFS3_OK)
	{
		Write(outStream, count);
		Write(outStream, stable);
		Write(outStream, verf);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " WRITE '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureCREATE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	CreateHow3 how;
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	NfsStat3 stat;
	FILE* pFile;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);
	std::string path = GetFullPath(dirName, fileName);
	Read(inStream, how);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS(dirName, &dir_wcc.before.attributes);

	pFile = _fsopen(path.c_str(), "wb", _SH_DENYWR);

	if (pFile != nullptr)
	{
		fclose(pFile);
		stat = NFS3_OK;
	}
	else
	{
		char buffer[BUFFER_SIZE];
		errno_t errorNumber = errno;
		strerror_s(buffer, BUFFER_SIZE, errorNumber);

		if (errorNumber == 2)
		{
			stat = NFS3ERR_STALE;
		}
		else if (errorNumber == 13)
		{
			stat = NFS3ERR_ACCES;
		}
		else
		{
			stat = NFS3ERR_IO;
		}
	}

	if (stat == NFS3_OK)
	{
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(outStream, stat);

	if (stat == NFS3_OK)
	{
		Write(outStream, obj);
		Write(outStream, objAttributes);
	}

	Write(outStream, dir_wcc);
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " CREATE '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureMKDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	SAttr3 attributes;
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	NfsStat3 stat;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);
	std::string path = GetFullPath(dirName, fileName);
	Read(inStream, attributes);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.before.attributes);

	const int result = _mkdir(path.c_str());
	if (result == 0)
	{
		stat = NFS3_OK;
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}
	else if (errno == EEXIST)
	{
		stat = NFS3ERR_EXIST;
	}
	else if (errno == ENOENT)
	{
		stat = NFS3ERR_NOENT;
	}
	else
	{
		stat = CheckFile(path);

		if (stat != NFS3_OK)
		{
			stat = NFS3ERR_IO;
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(outStream, stat);

	if (stat == NFS3_OK)
	{
		Write(outStream, obj);
		Write(outStream, objAttributes);
	}

	Write(outStream, dir_wcc);
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " MKDIR '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureSYMLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	NfsStat3 stat;

	DirOpArgs3 where;
	SymlinkData3 symlink;

	DWORD targetFileAttr;
	DWORD dwFlags;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);
	std::string path = GetFullPath(dirName, fileName);

	Read(inStream, symlink);

	_In_ LPTSTR lpSymlinkFileName = (LPTSTR)path.c_str(); // symlink (full path)

	// TODO: Maybe revisit this later for a cleaner solution
	// Convert target path to windows path format, maybe this could also be done
	// in a safer way by a combination of PathRelativePathTo and GetFullPathName.
	// Without this conversion nested folder symlinks do not work cross platform.
	std::string strFromChar;
	strFromChar.append(symlink.symlinkData.path); // target (should be relative path));
	std::replace(strFromChar.begin(), strFromChar.end(), '/', '\\');
	_In_ LPTSTR lpTargetFileName = const_cast<LPSTR>(strFromChar.c_str());

	std::string fullTargetPath = dirName + std::string("\\") + std::string(lpTargetFileName);

	// Relative path do not work with GetFileAttributes (directory are not recognized)
	// so we normalize the path before calling GetFileAttributes
	TCHAR fullTargetPathNormalized[MAX_PATH];
	_In_ LPTSTR fullTargetPathString = const_cast<LPSTR>(fullTargetPath.c_str());
	GetFullPathName(fullTargetPathString, MAX_PATH, fullTargetPathNormalized, NULL);
	targetFileAttr = GetFileAttributes(fullTargetPathNormalized);

	dwFlags = 0x0;
	if (targetFileAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		dwFlags = SYMBOLIC_LINK_FLAG_DIRECTORY;
	}

	BOOLEAN failed = CreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, dwFlags);

	if (failed != 0)
	{
		stat = NFS3_OK;
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}
	else
	{
		stat = NFS3ERR_IO;
		stat = CheckFile(path);
		if (stat != NFS3_OK)
		{
			stat = NFS3ERR_IO;
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(outStream, stat);

	if (stat == NFS3_OK)
	{
		Write(outStream, obj);
		Write(outStream, objAttributes);
	}

	Write(outStream, dir_wcc);
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " SYMLINK '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureMKNOD(IInputStream&, IOutputStream&, RPCParam& param)
{
	//TODO
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " MKNOD not implemented";

	return NFS3ERR_NOTSUPP;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureREMOVE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	WccData dir_wcc;
	NfsStat3 stat;
	unsigned long returnCode;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);
	std::string path = GetFullPath(dirName, fileName);
	stat = CheckFile(dirName, path);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS(dirName, &dir_wcc.before.attributes);

	if (stat == NFS3_OK)
	{
		DWORD fileAttr = GetFileAttributes(path.c_str());
		if ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) && (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT))
		{
			returnCode = RemoveFolder(path);
			if (returnCode != 0)
			{
				if (returnCode == ERROR_DIR_NOT_EMPTY)
				{
					stat = NFS3ERR_NOTEMPTY;
				}
				else
				{
					stat = NFS3ERR_IO;
				}
			}
		}
		else
		{
			if (!RemoveFile(path))
			{
				stat = NFS3ERR_IO;
			}
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, dir_wcc);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " REMOVE '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureRMDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	WccData dir_wcc;
	NfsStat3 stat;
	unsigned long returnCode;

	std::string dirName;
	std::string fileName;
	ReadDirectory(inStream, dirName, fileName);
	std::string path = GetFullPath(dirName, fileName);
	stat = CheckFile(dirName, path);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS(dirName, &dir_wcc.before.attributes);

	if (stat == NFS3_OK)
	{
		returnCode = RemoveFolder(path);
		if (returnCode != 0)
		{
			if (returnCode == ERROR_DIR_NOT_EMPTY)
			{
				stat = NFS3ERR_NOTEMPTY;
			}
			else
			{
				stat = NFS3ERR_IO;
			}
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS(dirName, &dir_wcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, dir_wcc);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " RMDIR '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureRENAME(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	WccData fromdir_wcc, todir_wcc;
	NfsStat3 stat;
	unsigned long returnCode;

	std::string dirFromName;
	std::string fileFromName;
	ReadDirectory(inStream, dirFromName, fileFromName);
	std::string pathFrom = GetFullPath(dirFromName, fileFromName);

	std::string dirToName;
	std::string fileToName;
	ReadDirectory(inStream, dirToName, fileToName);
	std::string pathTo = GetFullPath(dirToName, fileToName);

	stat = CheckFile(dirFromName, pathFrom);

	fromdir_wcc.before.attributesFollow = GetFileAttributesForNFS(dirFromName, &fromdir_wcc.before.attributes);
	todir_wcc.before.attributesFollow = GetFileAttributesForNFS(dirToName, &todir_wcc.before.attributes);

	if (FileExists(pathTo))
	{
		DWORD fileAttr = GetFileAttributes(pathTo.c_str());
		if ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) && (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT))
		{
			returnCode = RemoveFolder(pathTo);
			if (returnCode != 0)
			{
				if (returnCode == ERROR_DIR_NOT_EMPTY)
				{
					stat = NFS3ERR_NOTEMPTY;
				}
				else
				{
					stat = NFS3ERR_IO;
				}
			}
		}
		else
		{
			if (!RemoveFile(pathTo))
			{
				stat = NFS3ERR_IO;
			}
		}
	}

	if (stat == NFS3_OK)
	{
		errno_t errorNumber = RenameDirectory(pathFrom, pathTo);

		if (errorNumber != 0)
		{
			char buffer[BUFFER_SIZE];
			strerror_s(buffer, BUFFER_SIZE, errorNumber);

			if (errorNumber == 13)
			{
				stat = NFS3ERR_ACCES;
			}
			else
			{
				stat = NFS3ERR_IO;
			}
		}
	}

	fromdir_wcc.after.attributesFollow = GetFileAttributesForNFS(dirFromName, &fromdir_wcc.after.attributes);
	todir_wcc.after.attributesFollow = GetFileAttributesForNFS(dirToName, &todir_wcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, fromdir_wcc);
	Write(outStream, todir_wcc);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " RENAME '" << dirFromName
		<< "' to '" << dirToName << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	DirOpArgs3 link;
	std::string dirName;
	std::string fileName;
	NfsStat3 stat;
	PostOpAttr objAttributes;
	WccData dirWcc;

	std::string path = GetPath(inStream);
	ReadDirectory(inStream, dirName, fileName);

	std::string linkFullPath = GetFullPath(dirName, fileName);

	if (CreateHardLink(linkFullPath.c_str(), path.c_str(), NULL) == 0)
	{
		stat = NFS3ERR_IO;
	}
	stat = CheckFile(linkFullPath);
	if (stat == NFS3_OK)
	{
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);

		if (!objAttributes.attributesFollow)
		{
			stat = NFS3ERR_IO;
		}
	}

	dirWcc.after.attributesFollow = GetFileAttributesForNFS(dirName, &dirWcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, objAttributes);
	Write(outStream, dirWcc);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " LINK '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureREADDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	Cookie3 cookie;
	CookieVerf3 cookieverf;
	Count3 count;
	PostOpAttr dirAttributes;
	FileId3 fileid;
	NFSv3Filename name;
	bool eof;
	bool bFollows;
	NfsStat3 stat;
	char filePath[MAXPATHLEN];
	int nFound;
	intptr_t handle;
	struct _finddata_t fileinfo;
	unsigned int i, j;

	std::string path = GetPath(inStream);
	Read(inStream, cookie);
	Read(inStream, cookieverf);
	Read(inStream, count);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		dirAttributes.attributesFollow = GetFileAttributesForNFS(path, &dirAttributes.attributes);

		if (!dirAttributes.attributesFollow)
		{
			stat = NFS3ERR_IO;
		}
	}

	Write(outStream, stat);
	Write(outStream, dirAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, cookieverf);
		sprintf_s(filePath, "%s\\*", path.c_str());
		eof = true;
		handle = _findfirst(filePath, &fileinfo);
		bFollows = true;

		if (handle)
		{
			nFound = 0;

			for (i = (unsigned int)cookie; i > 0; i--)
			{
				nFound = _findnext(handle, &fileinfo);
			}

			// TODO: Implement this workaround correctly with the
			// count variable and not a fixed threshold of 10
			if (nFound == 0)
			{
				j = 10;

				do
				{
					Write(outStream, bFollows); //value follows
					sprintf_s(filePath, "%s\\%s", path.c_str(), fileinfo.name);
					fileid = GetFileID(filePath);
					Write(outStream, fileid); //file id
					name.Set(fileinfo.name);
					Write(outStream, name); //name
					++cookie;
					Write(outStream, cookie); //cookie
					if (--j == 0)
					{
						eof = false;
						break;
					}
				} while (_findnext(handle, &fileinfo) == 0);
			}

			_findclose(handle);
		}

		bFollows = false;
		Write(outStream, bFollows);
		Write(outStream, eof); //eof
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " READDIR '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureREADDIRPLUS(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	Cookie3 cookie;
	CookieVerf3 cookieverf;
	Count3 dircount, maxcount;
	PostOpAttr dirAttributes;
	FileId3 fileid;
	NFSv3Filename name;
	PostOpAttr nameAttributes;
	PostOpFH3 nameHandle;
	bool eof;
	NfsStat3 stat;
	char filePath[MAXPATHLEN];
	int nFound;
	intptr_t handle;
	struct _finddata_t fileinfo;
	unsigned int i, j;
	bool bFollows;

	std::string path = GetPath(inStream);
	Read(inStream, cookie);
	Read(inStream, cookieverf);
	Read(inStream, dircount);
	Read(inStream, maxcount);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		dirAttributes.attributesFollow = GetFileAttributesForNFS(path, &dirAttributes.attributes);

		if (!dirAttributes.attributesFollow)
		{
			stat = NFS3ERR_IO;
		}
	}

	Write(outStream, stat);
	Write(outStream, dirAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, cookieverf);
		sprintf_s(filePath, "%s\\*", path.c_str());
		handle = _findfirst(filePath, &fileinfo);
		eof = true;

		if (handle)
		{
			nFound = 0;

			for (i = (unsigned int)cookie; i > 0; i--)
			{
				nFound = _findnext(handle, &fileinfo);
			}

			if (nFound == 0)
			{
				bFollows = true;
				j = 10;

				do
				{
					Write(outStream, bFollows); //value follows
					sprintf_s(filePath, "%s\\%s", path.c_str(), fileinfo.name);
					fileid = GetFileID(filePath);
					Write(outStream, fileid); //file id
					name.Set(fileinfo.name);
					Write(outStream, name); //name
					++cookie;
					Write(outStream, cookie); //cookie
					nameAttributes.attributesFollow = GetFileAttributesForNFS(filePath, &nameAttributes.attributes);
					Write(outStream, nameAttributes);
					nameHandle.handleFollows = GetFileHandle(filePath, &nameHandle.handle);
					Write(outStream, nameHandle);

					if (--j == 0)
					{
						eof = false;
						break;
					}
				} while (_findnext(handle, &fileinfo) == 0);
			}

			_findclose(handle);
		}

		bFollows = false;
		Write(outStream, bFollows); //value follows
		Write(outStream, eof); //eof
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " READDIRPLUS '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureFSSTAT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	PostOpAttr objAttributes{};
	Size3 tbytes = 0, fbytes = 0, abytes = 0, tfiles = 0, ffiles = 0, afiles = 0;
	uint32_t invarsec = 0;

	NfsStat3 stat;

	std::string path = GetPath(inStream);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);

		if (objAttributes.attributesFollow
			&& GetDiskFreeSpaceEx(path.c_str(), (PULARGE_INTEGER)&fbytes, (PULARGE_INTEGER)&tbytes, (PULARGE_INTEGER)&abytes))
		{
			//tfiles = 99999999999;
			//ffiles = 99999999999;
			//afiles = 99999999999;
			invarsec = 0;
		}
		else {
			stat = NFS3ERR_IO;
		}
	}

	Write(outStream, stat);
	Write(outStream, objAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, tbytes);
		Write(outStream, fbytes);
		Write(outStream, abytes);
		Write(outStream, tfiles);
		Write(outStream, ffiles);
		Write(outStream, afiles);
		Write(outStream, invarsec);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " FSSTAT '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureFSINFO(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	PostOpAttr objAttributes{};
	uint32_t rtmax = 0, rtpref = 0, rtmult = 0, wtmax = 0, wtpref = 0, wtmult = 0, dtpref = 0;
	Size3 maxfilesize = 0;
	NFSTime3 timeDelta{};
	uint32_t properties = 0;
	NfsStat3 stat{};

	std::string path = GetPath(inStream);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);

		if (objAttributes.attributesFollow)
		{
			rtmax = 65536;
			rtpref = 32768;
			rtmult = 4096;
			wtmax = 65536;
			wtpref = 32768;
			wtmult = 4096;
			dtpref = 8192;
			maxfilesize = 0x7FFFFFFFFFFFFFFF;
			timeDelta.seconds = 1;
			timeDelta.nseconds = 0;
			properties = FSF3_LINK | FSF3_SYMLINK | FSF3_CANSETTIME;
		}
		else
		{
			stat = NFS3ERR_SERVERFAULT;
		}
	}

	Write(outStream, stat);
	Write(outStream, objAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, rtmax);
		Write(outStream, rtpref);
		Write(outStream, rtmult);
		Write(outStream, wtmax);
		Write(outStream, wtpref);
		Write(outStream, wtmult);
		Write(outStream, dtpref);
		Write(outStream, maxfilesize);
		Write(outStream, timeDelta);
		Write(outStream, properties);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " FSINFO '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedurePATHCONF(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	PostOpAttr objAttributes{};
	NfsStat3 stat{};
	uint32_t linkMax = 0, nameMax = 0;
	bool noTrunc = false, chownRestricted = false, caseInsensitive = false, casePreserving = false;

	std::string path = GetPath(inStream);
	stat = CheckFile(path);

	if (stat == NFS3_OK)
	{
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);

		if (objAttributes.attributesFollow)
		{
			linkMax = 1023;
			nameMax = 255;
			noTrunc = true;
			chownRestricted = true;
			caseInsensitive = true;
			casePreserving = true;
		}
		else
		{
			stat = NFS3ERR_SERVERFAULT;
		}
	}

	Write(outStream, stat);
	Write(outStream, objAttributes);

	if (stat == NFS3_OK)
	{
		Write(outStream, linkMax);
		Write(outStream, nameMax);
		Write(outStream, noTrunc);
		Write(outStream, chownRestricted);
		Write(outStream, caseInsensitive);
		Write(outStream, casePreserving);
	}

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " PATHCONF '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureCOMMIT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	std::string path;
	int handleId;
	Offset3 offset;
	Count3 count;
	WccData fileWcc;
	NfsStat3 stat;
	NFSv3FileHandle file;
	WriteVerf3 verf;

	Read(inStream, file);
	GetFilePath(file.contents, path);

	// offset and count are unused
	// offset never was anything else than 0 in my tests
	// count does not matter in the way COMMIT is implemented here
	// to fulfill the spec this should be improved
	Read(inStream, offset);
	Read(inStream, count);

	fileWcc.before.attributesFollow = GetFileAttributesForNFS(path, &fileWcc.before.attributes);

	handleId = *(unsigned int*)file.contents;

	if (unstableStorageFile.count(handleId) != 0)
	{
		if (unstableStorageFile[handleId] != NULL)
		{
			fclose(unstableStorageFile[handleId]);
			unstableStorageFile.erase(handleId);
			stat = NFS3_OK;
		}
		else
		{
			stat = NFS3ERR_IO;
		}
	}
	else
	{
		stat = NFS3_OK;
	}

	fileWcc.after.attributesFollow = GetFileAttributesForNFS(path, &fileWcc.after.attributes);

	Write(outStream, stat);
	Write(outStream, fileWcc);
	// verf should be the timestamp the server startet to notice reboots
	verf = 0;
	Write(outStream, verf);

	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " COMMIT '" << path << "': " << NfsStatToString(stat);
	return stat;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::ProcedureNOIMP(IInputStream&, IOutputStream&, RPCParam& param)
{
	BOOST_LOG_TRIVIAL(debug) << "NFS3 " << param.remoteAddr << " NOIMP";
	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
std::string NFS3Prog::GetPath(IInputStream& inStream)
{
	NFSv3FileHandle object{};
	Read(inStream, object);

	std::string path;
	if (!GetFilePath(object.contents, path))
	{
		throw std::runtime_error("file handle is invalid");
	}
	return path;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::ReadDirectory(IInputStream& inStream, std::string& dirName, std::string& fileName)
{
	DirOpArgs3 fileRequest{};
	Read(inStream, fileRequest);

	if (GetFilePath(fileRequest.dir.contents, dirName))
	{
		fileName = std::string(fileRequest.name.name);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
std::string NFS3Prog::GetFullPath(const std::string& dirName, const std::string& fileName)
{
	return dirName + "\\" + fileName;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::CheckFile(const std::string& fullPath)
{
	if (_access(fullPath.c_str(), 0) != 0)
	{
		return NFS3ERR_NOENT;
	}

	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
NfsStat3 NFS3Prog::CheckFile(const std::string& directory, const std::string& fullPath)
{
	// FileExists will not work for the root of a drive, e.g. \\?\D:\, therefore check if it is a drive root with GetDriveType
	if (!FileExists(directory) && GetDriveType(directory.c_str()) < 2)
	{
		return NFS3ERR_STALE;
	}

	if (!FileExists(fullPath))
	{
		return NFS3ERR_NOENT;
	}

	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetFileHandle(const std::string& path, NFSv3FileHandle* pObject)
{
	const auto handle = ::GetFileHandle(path);
	if (!handle)
	{
		BOOST_LOG_TRIVIAL(error) << "no file handle for path " << path;
		return false;
	}

	auto err = memcpy_s(pObject->contents, NFS3_FHSIZE, ::GetFileHandle(path), pObject->length);
	if (err != 0) {
		BOOST_LOG_TRIVIAL(error) << "failed to copy file handle for path " << path << ": errno=" << err;
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetFileAttributesForNFS(const std::string& path, WccAttr* pAttr)
{
	struct stat data;

	if (stat(path.c_str(), &data) != 0)
	{
		return false;
	}

	pAttr->size = data.st_size;
	pAttr->mtime.seconds = (unsigned int)data.st_mtime;
	pAttr->mtime.nseconds = 0;
	// TODO: This needs to be tested (not called on my setup)
	// This seems to be the changed time, not creation time.
	//pAttr->ctime.seconds = data.st_ctime;
	pAttr->ctime.seconds = (unsigned int)data.st_mtime;
	pAttr->ctime.nseconds = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetFileAttributesForNFS(const std::string& path, FAttr3* pAttr)
{
	const DWORD fileAttr = GetFileAttributes(path.c_str());
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	DWORD dwFlagsAndAttributes = 0;
	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		pAttr->type = NF3DIR;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_DIRECTORY | FILE_FLAG_BACKUP_SEMANTICS;
	}
	else if (fileAttr & FILE_ATTRIBUTE_ARCHIVE)
	{
		pAttr->type = NF3REG;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_OVERLAPPED;
	}
	else if (fileAttr & FILE_ATTRIBUTE_NORMAL)
	{
		pAttr->type = NF3REG;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
	}
	else
	{
		pAttr->type = 0;
	}

	if (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT)
	{
		pAttr->type = NF3LNK;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_REPARSE_POINT | FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS;
	}

	const HANDLE hFile = CreateFile(path.c_str(), FILE_READ_EA, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	BY_HANDLE_FILE_INFORMATION lpFileInformation;
	GetFileInformationByHandle(hFile, &lpFileInformation);
	CloseHandle(hFile);
	pAttr->mode = 0;

	// Set execution right for all
	pAttr->mode |= 0x49;

	// Set read right for all
	pAttr->mode |= 0x124;

	//if ((lpFileInformation.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0) {
	pAttr->mode |= 0x92;
	//}

	ULONGLONG fileSize = lpFileInformation.nFileSizeHigh;
	fileSize <<= sizeof(lpFileInformation.nFileSizeHigh) * 8;
	fileSize |= lpFileInformation.nFileSizeLow;

	pAttr->nlink = lpFileInformation.nNumberOfLinks;
	pAttr->uid = m_uid;
	pAttr->gid = m_gid;
	pAttr->size = fileSize;
	pAttr->used = pAttr->size;
	pAttr->rdev.specdata1 = 0;
	pAttr->rdev.specdata2 = 0;
	pAttr->fsid = 7; //NTFS //4;
	pAttr->fileid = GetFileID(path);
	pAttr->atime.seconds = FileTimeToPOSIX(lpFileInformation.ftLastAccessTime);
	pAttr->atime.nseconds = 0;
	pAttr->mtime.seconds = FileTimeToPOSIX(lpFileInformation.ftLastWriteTime);
	pAttr->mtime.nseconds = 0;
	// This seems to be the changed time, not creation time
	pAttr->ctime.seconds = FileTimeToPOSIX(lpFileInformation.ftLastWriteTime);
	pAttr->ctime.nseconds = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////
UINT32 NFS3Prog::FileTimeToPOSIX(FILETIME ft)
{
	// takes the last modified date
	LARGE_INTEGER date{}, adjust{};
	date.HighPart = ft.dwHighDateTime;
	date.LowPart = ft.dwLowDateTime;

	// 100-nanoseconds = milliseconds * 10000
	adjust.QuadPart = 11644473600000 * 10000;

	// removes the diff between 1970 and 1601
	date.QuadPart -= adjust.QuadPart;

	// converts back from 100-nanoseconds to seconds
	return (unsigned int)(date.QuadPart / 10000000);
}