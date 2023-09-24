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
#include "shlwapi.h"
#define BUFFER_SIZE 1000

struct Opaque
{
	uint32 length;
	unsigned char* contents;

	Opaque();
	Opaque(uint32 len);
	virtual ~Opaque();
	virtual void SetSize(uint32 len);
};

struct NFSv3FileHandle : public Opaque
{
	NFSv3FileHandle();
	~NFSv3FileHandle() = default;
};

struct NFSv3Filename : public Opaque
{
	char* name;

	NFSv3Filename();
	~NFSv3Filename() = default;
	void SetSize(uint32 len);
	void Set(char* str);
};

struct NFSv3Path : public Opaque
{
	char* path;

	NFSv3Path();
	~NFSv3Path() = default;
	void SetSize(uint32 len);
	void Set(char* str);
};

struct SpecData3
{
	uint32 specdata1;
	uint32 specdata2;
};

struct NFSTime3
{
	uint32 seconds;
	uint32 nseconds;
};

struct SAttrGuard3
{
	bool check;
	NFSTime3 objCtime;
};

struct FAttr3
{
	ftype3 type;
	mode3 mode;
	uint32 nlink;
	uid3 uid;
	gid3 gid;
	size3 size;
	size3 used;
	SpecData3 rdev;
	uint64 fsid;
	fileid3 fileid;
	NFSTime3 atime;
	NFSTime3 mtime;
	NFSTime3 ctime;
};

struct PostOpAttr
{
	bool attributesFollow;
	FAttr3 attributes;
};

struct WccAttr
{
	size3 size;
	NFSTime3 mtime;
	NFSTime3 ctime;
};

struct PreOpAttr
{
	bool attributesFollow;
	WccAttr attributes;
};

struct WccData
{
	PreOpAttr before;
	PostOpAttr after;
};

struct PostOpFH3
{
	bool handleFollows;
	NFSv3FileHandle handle;
};

struct SetMode3
{
	bool setIt;
	mode3 mode;
};

struct SetUid3
{
	bool setIt;
	uid3 uid;
};

struct SetGid3
{
	bool setIt;
	gid3 gid;
};

struct SetSize3
{
	bool setIt;
	size3 size;
};

struct SetAtime
{
	time_how setIt;
	NFSTime3 atime;
};

struct SetMtime
{
	time_how setIt;
	NFSTime3 mtime;
};

struct SAttr3
{
	SetMode3 mode;
	SetUid3 uid;
	SetGid3 gid;
	SetSize3 size;
	SetAtime atime;
	SetMtime mtime;
};

struct DirOpArgs3
{
	NFSv3FileHandle dir;
	NFSv3Filename name;
};

struct CreateHow3
{
	createmode3 mode;
	SAttr3 objAttributes;
	createverf3 verf;
};

struct SymlinkData3
{
	SAttr3 symlinkAttributes;
	NFSv3Path symlinkData;
};

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

enum
{
	ACCESS3_READ = 0x0001,
	ACCESS3_LOOKUP = 0x0002,
	ACCESS3_MODIFY = 0x0004,
	ACCESS3_EXTEND = 0x0008,
	ACCESS3_DELETE = 0x0010,
	ACCESS3_EXECUTE = 0x0020
};

enum
{
	FSF3_LINK = 0x0001,
	FSF3_SYMLINK = 0x0002,
	FSF3_HOMOGENEOUS = 0x0008,
	FSF3_CANSETTIME = 0x0010
};

enum
{
	UNSTABLE = 0,
	DATA_SYNC = 1,
	FILE_SYNC = 2
};

enum
{
	DONT_CHANGE = 0,
	SET_TO_SERVER_TIME = 1,
	SET_TO_CLIENT_TIME = 2
};

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
Opaque::Opaque(uint32 len)
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
void Opaque::SetSize(uint32 len)
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
void NFSv3Filename::SetSize(uint32 len)
{
	Opaque::SetSize(len + 1);
	length = len;
	name = (char*)contents;
}

/////////////////////////////////////////////////////////////////////
void NFSv3Filename::Set(char* str)
{
	SetSize(static_cast<uint32>(strlen(str)));
	strcpy_s(name, (strlen(str) + 1), str);
}

/////////////////////////////////////////////////////////////////////
NFSv3Path::NFSv3Path()
	: Opaque()
	, path(nullptr)
{}

/////////////////////////////////////////////////////////////////////
void NFSv3Path::SetSize(uint32 len)
{
	Opaque::SetSize(len + 1);
	length = len;
	path = (char*)contents;
}

/////////////////////////////////////////////////////////////////////
void NFSv3Path::Set(char* str)
{
	SetSize(static_cast<uint32>(strlen(str)));
	strcpy_s(path, (strlen(str) + 1), str);
}

typedef nfsstat3(NFS3Prog::* PPROC)(void);

/////////////////////////////////////////////////////////////////////
NFS3Prog::NFS3Prog(unsigned int uid, unsigned int gid, bool enableLog)
	: RPCProg()
	, m_uid(uid)
	, m_gid(gid)
	, m_inStream(nullptr)
	, m_outStream(nullptr)
	, m_param(nullptr)
	, m_result(0)
{
	EnableLog(enableLog);
}

/////////////////////////////////////////////////////////////////////
int NFS3Prog::Process(IInputStream* pInStream, IOutputStream* pOutStream, ProcessParam* pParam)
{
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

	nfsstat3 stat = 0;

	struct tm current;
	time_t now;

	time(&now);
	localtime_s(&current, &now);

	PrintLog("[%02d:%02d:%02d] NFS ", current.tm_hour, current.tm_min, current.tm_sec);

	if (pParam->nProc >= sizeof(pf) / sizeof(PPROC))
	{
		ProcedureNOIMP();
		PrintLog("\n");

		return PRC_NOTIMP;
	}

	m_inStream = pInStream;
	m_outStream = pOutStream;
	m_param = pParam;
	m_result = PRC_OK;

	try
	{
		stat = (this->*pf[pParam->nProc])();
	}
	catch (const std::runtime_error& re)
	{
		m_result = PRC_FAIL;
		PrintLog("Runtime error: ");
		PrintLog(re.what());
	}
	catch (const std::exception& ex)
	{
		m_result = PRC_FAIL;
		PrintLog("Exception: ");
		PrintLog(ex.what());
	}
	catch (...)
	{
		m_result = PRC_FAIL;
		PrintLog("Unknown failure: Possible memory corruption");
	}

	PrintLog(" ");

	switch (stat)
	{
	case NFS3_OK:
		PrintLog("OK");
		break;
	case NFS3ERR_PERM:
		PrintLog("PERM");
		break;
	case NFS3ERR_NOENT:
		PrintLog("NOENT");
		break;
	case NFS3ERR_IO:
		PrintLog("IO");
		break;
	case NFS3ERR_NXIO:
		PrintLog("NXIO");
		break;
	case NFS3ERR_ACCES:
		PrintLog("ACCESS");
		break;
	case NFS3ERR_EXIST:
		PrintLog("EXIST");
		break;
	case NFS3ERR_XDEV:
		PrintLog("XDEV");
		break;
	case NFS3ERR_NODEV:
		PrintLog("NODEV");
		break;
	case NFS3ERR_NOTDIR:
		PrintLog("NOTDIR");
		break;
	case NFS3ERR_ISDIR:
		PrintLog("ISDIR");
		break;
	case NFS3ERR_INVAL:
		PrintLog("INVAL");
		break;
	case NFS3ERR_FBIG:
		PrintLog("FBIG");
		break;
	case NFS3ERR_NOSPC:
		PrintLog("NOSPC");
		break;
	case NFS3ERR_ROFS:
		PrintLog("ROFS");
		break;
	case NFS3ERR_MLINK:
		PrintLog("MLINK");
		break;
	case NFS3ERR_NAMETOOLONG:
		PrintLog("NAMETOOLONG");
		break;
	case NFS3ERR_NOTEMPTY:
		PrintLog("NOTEMPTY");
		break;
	case NFS3ERR_DQUOT:
		PrintLog("DQUOT");
		break;
	case NFS3ERR_STALE:
		PrintLog("STALE");
		break;
	case NFS3ERR_REMOTE:
		PrintLog("REMOTE");
		break;
	case NFS3ERR_BADHANDLE:
		PrintLog("BADHANDLE");
		break;
	case NFS3ERR_NOT_SYNC:
		PrintLog("NOT_SYNC");
		break;
	case NFS3ERR_BAD_COOKIE:
		PrintLog("BAD_COOKIE");
		break;
	case NFS3ERR_NOTSUPP:
		PrintLog("NOTSUPP");
		break;
	case NFS3ERR_TOOSMALL:
		PrintLog("TOOSMALL");
		break;
	case NFS3ERR_SERVERFAULT:
		PrintLog("SERVERFAULT");
		break;
	case NFS3ERR_BADTYPE:
		PrintLog("BADTYPE");
		break;
	case NFS3ERR_JUKEBOX:
		PrintLog("JUKEBOX");
		break;
	default:
		assert(false);
		break;
	}

	PrintLog("\n");

	return m_result;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureNULL()
{
	PrintLog("NULL");
	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureGETATTR()
{
	std::string path;
	FAttr3 attributes;
	nfsstat3 stat;

	PrintLog("GETATTR");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	stat = CheckFile(cStr);

	if (stat == NFS3ERR_NOENT)
	{
		stat = NFS3ERR_STALE;
	}
	else if (stat == NFS3_OK)
	{
		if (!GetFileAttributesForNFS(cStr, &attributes))
		{
			stat = NFS3ERR_IO;
		}
	}

	Write(&stat);

	if (stat == NFS3_OK)
	{
		Write(&attributes);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureSETATTR()
{
	std::string path;
	SAttr3 newAttributes;
	SAttrGuard3 guard;
	WccData objWcc;
	nfsstat3 stat;
	int mode;
	FILE* file;
	HANDLE fileHandle;
	FILETIME fileTime;
	SYSTEMTIME systemTime;

	PrintLog("SETATTR");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&newAttributes);
	Read(&guard);
	stat = CheckFile(cStr);
	objWcc.before.attributesFollow = GetFileAttributesForNFS(cStr, &objWcc.before.attributes);

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

			if (_chmod(cStr, mode) != 0)
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
			fileHandle = CreateFile(cStr, FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
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
			file = _fsopen(cStr, "r+b", _SH_DENYWR);
			if (file != nullptr)
			{
				int filedes = _fileno(file);
				_chsize_s(filedes, newAttributes.size.size);
				fclose(file);
			}
		}
	}

	objWcc.after.attributesFollow = GetFileAttributesForNFS(cStr, &objWcc.after.attributes);

	Write(&stat);
	Write(&objWcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureLOOKUP()
{
	char* path;
	NFSv3FileHandle object;
	PostOpAttr fileAttributes;
	PostOpAttr dirAttributes;
	nfsstat3 stat;

	PrintLog("LOOKUP");

	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);

	path = GetFullPath(dirName, fileName);
	stat = CheckFile((char*)dirName.c_str(), path);

	if (stat == NFS3_OK)
	{
		GetFileHandle(path, &object);
		fileAttributes.attributesFollow = GetFileAttributesForNFS(path, &fileAttributes.attributes);
	}

	dirAttributes.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dirAttributes.attributes);

	Write(&stat);

	if (stat == NFS3_OK)
	{
		Write(&object);
		Write(&fileAttributes);
	}

	Write(&dirAttributes);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureACCESS()
{
	std::string path;
	uint32 access;
	PostOpAttr objAttributes;
	nfsstat3 stat;

	PrintLog("ACCESS");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&access);
	stat = CheckFile(cStr);

	if (stat == NFS3ERR_NOENT)
	{
		stat = NFS3ERR_STALE;
	}

	objAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &objAttributes.attributes);

	Write(&stat);
	Write(&objAttributes);

	if (stat == NFS3_OK)
	{
		Write(&access);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureREADLINK(void)
{
	PrintLog("READLINK");
	std::string path;

	PostOpAttr symlinkAttributes;
	NFSv3Path data = NFSv3Path();

	//opaque data;
	nfsstat3 stat;

	HANDLE hFile;
	REPARSE_DATA_BUFFER* lpOutBuffer;
	lpOutBuffer = (REPARSE_DATA_BUFFER*)malloc(MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
	DWORD bytesReturned;

	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	stat = CheckFile(cStr);
	if (stat == NFS3_OK) {
		hFile = CreateFile(cStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_REPARSE_POINT | FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);

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
							PathRelativePathTo(szOut, cStr, FILE_ATTRIBUTE_DIRECTORY, target, FILE_ATTRIBUTE_DIRECTORY);
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
						PathRelativePathTo(szOut, cStr, FILE_ATTRIBUTE_DIRECTORY, target.c_str(), FILE_ATTRIBUTE_DIRECTORY);
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

	symlinkAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &symlinkAttributes.attributes);

	Write(&stat);
	Write(&symlinkAttributes);
	if (stat == NFS3_OK) {
		Write(&data);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureREAD(void)
{
	std::string path;
	offset3 offset;
	count3 count;
	PostOpAttr file_attributes;
	bool eof;
	Opaque data;
	nfsstat3 stat;
	FILE* pFile;

	PrintLog("READ");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&offset);
	Read(&count);
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		data.SetSize(count);
		pFile = _fsopen(cStr, "rb", _SH_DENYWR);

		if (pFile != NULL) {
			_fseeki64(pFile, offset, SEEK_SET);
			count = static_cast<count3>(fread(data.contents, sizeof(char), count, pFile));
			eof = fgetc(pFile) == EOF;
			fclose(pFile);
		}
		else {
			char buffer[BUFFER_SIZE];
			errno_t errorNumber = errno;
			strerror_s(buffer, BUFFER_SIZE, errorNumber);
			PrintLog(buffer);

			if (errorNumber == 13) {
				stat = NFS3ERR_ACCES;
			}
			else {
				stat = NFS3ERR_IO;
			}
		}
	}

	file_attributes.attributesFollow = GetFileAttributesForNFS(cStr, &file_attributes.attributes);

	Write(&stat);
	Write(&file_attributes);

	if (stat == NFS3_OK) {
		Write(&count);
		Write(&eof);
		Write(&data);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureWRITE(void)
{
	std::string path;
	offset3 offset;
	count3 count;
	stable_how stable;
	Opaque data;
	WccData file_wcc;
	writeverf3 verf;
	nfsstat3 stat;
	FILE* pFile;

	PrintLog("WRITE");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&offset);
	Read(&count);
	Read(&stable);
	Read(&data);
	stat = CheckFile(cStr);

	file_wcc.before.attributesFollow = GetFileAttributesForNFS(cStr, &file_wcc.before.attributes);

	if (stat == NFS3_OK) {
		if (stable == UNSTABLE) {
			NFSv3FileHandle handle;
			GetFileHandle(cStr, &handle);
			int handleId = *(unsigned int*)handle.contents;

			if (unstableStorageFile.count(handleId) == 0) {
				pFile = _fsopen(cStr, "r+b", _SH_DENYWR);
				if (pFile != NULL) {
					unstableStorageFile.insert(std::make_pair(handleId, pFile));
				}
			}
			else {
				pFile = unstableStorageFile[handleId];
			}

			if (pFile != NULL) {
				_fseeki64(pFile, offset, SEEK_SET);
				count = static_cast<count3>(fwrite(data.contents, sizeof(char), data.length, pFile));
			}
			else {
				char buffer[BUFFER_SIZE];
				errno_t errorNumber = errno;
				strerror_s(buffer, BUFFER_SIZE, errorNumber);
				PrintLog(buffer);

				if (errorNumber == 13) {
					stat = NFS3ERR_ACCES;
				}
				else {
					stat = NFS3ERR_IO;
				}
			}
			// this should not be zero but a timestamp (process start time) instead
			verf = 0;
			// we can reuse this, because no physical write has happend
			file_wcc.after.attributesFollow = file_wcc.before.attributesFollow;
		}
		else {
			pFile = _fsopen(cStr, "r+b", _SH_DENYWR);

			if (pFile != NULL) {
				_fseeki64(pFile, offset, SEEK_SET);
				count = static_cast<count3>(fwrite(data.contents, sizeof(char), data.length, pFile));
				fclose(pFile);
			}
			else {
				char buffer[BUFFER_SIZE];
				errno_t errorNumber = errno;
				strerror_s(buffer, BUFFER_SIZE, errorNumber);
				PrintLog(buffer);

				if (errorNumber == 13) {
					stat = NFS3ERR_ACCES;
				}
				else {
					stat = NFS3ERR_IO;
				}
			}

			stable = FILE_SYNC;
			verf = 0;

			file_wcc.after.attributesFollow = GetFileAttributesForNFS(cStr, &file_wcc.after.attributes);
		}
	}

	Write(&stat);
	Write(&file_wcc);

	if (stat == NFS3_OK) {
		Write(&count);
		Write(&stable);
		Write(&verf);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureCREATE(void)
{
	char* path;
	CreateHow3 how;
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	nfsstat3 stat;
	FILE* pFile;

	PrintLog("CREATE");
	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);
	path = GetFullPath(dirName, fileName);
	Read(&how);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.before.attributes);

	pFile = _fsopen(path, "wb", _SH_DENYWR);

	if (pFile != NULL) {
		fclose(pFile);
		stat = NFS3_OK;
	}
	else {
		char buffer[BUFFER_SIZE];
		errno_t errorNumber = errno;
		strerror_s(buffer, BUFFER_SIZE, errorNumber);
		PrintLog(buffer);

		if (errorNumber == 2) {
			stat = NFS3ERR_STALE;
		}
		else if (errorNumber == 13) {
			stat = NFS3ERR_ACCES;
		}
		else {
			stat = NFS3ERR_IO;
		}
	}

	if (stat == NFS3_OK) {
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);

	if (stat == NFS3_OK) {
		Write(&obj);
		Write(&objAttributes);
	}

	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureMKDIR(void)
{
	char* path;
	SAttr3 attributes;
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	nfsstat3 stat;

	PrintLog("MKDIR");

	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);
	path = GetFullPath(dirName, fileName);
	Read(&attributes);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.before.attributes);

	int result = _mkdir(path);

	if (result == 0) {
		stat = NFS3_OK;
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}
	else if (errno == EEXIST) {
		PrintLog("Directory already exists.");
		stat = NFS3ERR_EXIST;
	}
	else if (errno == ENOENT) {
		stat = NFS3ERR_NOENT;
	}
	else {
		stat = CheckFile(path);

		if (stat != NFS3_OK) {
			stat = NFS3ERR_IO;
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);

	if (stat == NFS3_OK) {
		Write(&obj);
		Write(&objAttributes);
	}

	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureSYMLINK(void)
{
	PrintLog("SYMLINK");

	char* path;
	PostOpFH3 obj;
	PostOpAttr objAttributes;
	WccData dir_wcc;
	nfsstat3 stat;

	DirOpArgs3 where;
	SymlinkData3 symlink;

	DWORD targetFileAttr;
	DWORD dwFlags;

	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);
	path = GetFullPath(dirName, fileName);

	Read(&symlink);

	_In_ LPTSTR lpSymlinkFileName = path; // symlink (full path)

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
	if (targetFileAttr & FILE_ATTRIBUTE_DIRECTORY) {
		dwFlags = SYMBOLIC_LINK_FLAG_DIRECTORY;
	}

	BOOLEAN failed = CreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, dwFlags);

	if (failed != 0) {
		stat = NFS3_OK;
		obj.handleFollows = GetFileHandle(path, &obj.handle);
		objAttributes.attributesFollow = GetFileAttributesForNFS(path, &objAttributes.attributes);
	}
	else {
		stat = NFS3ERR_IO;
		PrintLog("An error occurs or file already exists.");
		stat = CheckFile(path);
		if (stat != NFS3_OK) {
			stat = NFS3ERR_IO;
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);

	if (stat == NFS3_OK) {
		Write(&obj);
		Write(&objAttributes);
	}

	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureMKNOD(void)
{
	//TODO
	PrintLog("MKNOD");

	return NFS3ERR_NOTSUPP;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureREMOVE(void)
{
	char* path;
	WccData dir_wcc;
	nfsstat3 stat;
	unsigned long returnCode;

	PrintLog("REMOVE");

	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);
	path = GetFullPath(dirName, fileName);
	stat = CheckFile((char*)dirName.c_str(), path);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.before.attributes);

	if (stat == NFS3_OK) {
		DWORD fileAttr = GetFileAttributes(path);
		if ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) && (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT)) {
			returnCode = RemoveFolder(path);
			if (returnCode != 0) {
				if (returnCode == ERROR_DIR_NOT_EMPTY) {
					stat = NFS3ERR_NOTEMPTY;
				}
				else {
					stat = NFS3ERR_IO;
				}
			}
		}
		else {
			if (!RemoveFile(path)) {
				stat = NFS3ERR_IO;
			}
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);
	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureRMDIR(void)
{
	char* path;
	WccData dir_wcc;
	nfsstat3 stat;
	unsigned long returnCode;

	PrintLog("RMDIR");

	std::string dirName;
	std::string fileName;
	ReadDirectory(dirName, fileName);
	path = GetFullPath(dirName, fileName);
	stat = CheckFile((char*)dirName.c_str(), path);

	dir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.before.attributes);

	if (stat == NFS3_OK) {
		returnCode = RemoveFolder(path);
		if (returnCode != 0) {
			if (returnCode == ERROR_DIR_NOT_EMPTY) {
				stat = NFS3ERR_NOTEMPTY;
			}
			else {
				stat = NFS3ERR_IO;
			}
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);
	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureRENAME(void)
{
	char pathFrom[MAXPATHLEN], * pathTo;
	WccData fromdir_wcc, todir_wcc;
	nfsstat3 stat;
	unsigned long returnCode;

	PrintLog("RENAME");

	std::string dirFromName;
	std::string fileFromName;
	ReadDirectory(dirFromName, fileFromName);
	strcpy_s(pathFrom, GetFullPath(dirFromName, fileFromName));

	std::string dirToName;
	std::string fileToName;
	ReadDirectory(dirToName, fileToName);
	pathTo = GetFullPath(dirToName, fileToName);

	stat = CheckFile((char*)dirFromName.c_str(), pathFrom);

	fromdir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirFromName.c_str(), &fromdir_wcc.before.attributes);
	todir_wcc.before.attributesFollow = GetFileAttributesForNFS((char*)dirToName.c_str(), &todir_wcc.before.attributes);

	if (FileExists(pathTo)) {
		DWORD fileAttr = GetFileAttributes(pathTo);
		if ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) && (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT)) {
			returnCode = RemoveFolder(pathTo);
			if (returnCode != 0) {
				if (returnCode == ERROR_DIR_NOT_EMPTY) {
					stat = NFS3ERR_NOTEMPTY;
				}
				else {
					stat = NFS3ERR_IO;
				}
			}
		}
		else {
			if (!RemoveFile(pathTo)) {
				stat = NFS3ERR_IO;
			}
		}
	}

	if (stat == NFS3_OK) {
		errno_t errorNumber = RenameDirectory(pathFrom, pathTo);

		if (errorNumber != 0) {
			char buffer[BUFFER_SIZE];
			strerror_s(buffer, BUFFER_SIZE, errorNumber);
			PrintLog(buffer);

			if (errorNumber == 13) {
				stat = NFS3ERR_ACCES;
			}
			else {
				stat = NFS3ERR_IO;
			}
		}
	}

	fromdir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirFromName.c_str(), &fromdir_wcc.after.attributes);
	todir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirToName.c_str(), &todir_wcc.after.attributes);

	Write(&stat);
	Write(&fromdir_wcc);
	Write(&todir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureLINK(void)
{
	PrintLog("LINK");
	std::string path;
	DirOpArgs3 link;
	std::string dirName;
	std::string fileName;
	nfsstat3 stat;
	PostOpAttr objAttributes;
	WccData dir_wcc;

	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	ReadDirectory(dirName, fileName);

	char* linkFullPath = GetFullPath(dirName, fileName);

	//TODO: Improve checks here, cStr may be NULL because handle is invalid
	if (CreateHardLink(linkFullPath, cStr, NULL) == 0) {
		stat = NFS3ERR_IO;
	}
	stat = CheckFile(linkFullPath);
	if (stat == NFS3_OK) {
		objAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &objAttributes.attributes);

		if (!objAttributes.attributesFollow) {
			stat = NFS3ERR_IO;
		}
	}

	dir_wcc.after.attributesFollow = GetFileAttributesForNFS((char*)dirName.c_str(), &dir_wcc.after.attributes);

	Write(&stat);
	Write(&objAttributes);
	Write(&dir_wcc);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureREADDIR(void)
{
	std::string path;
	cookie3 cookie;
	cookieverf3 cookieverf;
	count3 count;
	PostOpAttr dir_attributes;
	fileid3 fileid;
	NFSv3Filename name;
	bool eof;
	bool bFollows;
	nfsstat3 stat;
	char filePath[MAXPATHLEN];
	int nFound;
	intptr_t handle;
	struct _finddata_t fileinfo;
	unsigned int i, j;

	PrintLog("READDIR");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&cookie);
	Read(&cookieverf);
	Read(&count);
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		dir_attributes.attributesFollow = GetFileAttributesForNFS(cStr, &dir_attributes.attributes);

		if (!dir_attributes.attributesFollow) {
			stat = NFS3ERR_IO;
		}
	}

	Write(&stat);
	Write(&dir_attributes);

	if (stat == NFS3_OK) {
		Write(&cookieverf);
		sprintf_s(filePath, "%s\\*", cStr);
		eof = true;
		handle = _findfirst(filePath, &fileinfo);
		bFollows = true;

		if (handle) {
			nFound = 0;

			for (i = (unsigned int)cookie; i > 0; i--) {
				nFound = _findnext(handle, &fileinfo);
			}

			// TODO: Implement this workaround correctly with the
			// count variable and not a fixed threshold of 10
			if (nFound == 0) {
				j = 10;

				do {
					Write(&bFollows); //value follows
					sprintf_s(filePath, "%s\\%s", cStr, fileinfo.name);
					fileid = GetFileID(filePath);
					Write(&fileid); //file id
					name.Set(fileinfo.name);
					Write(&name); //name
					++cookie;
					Write(&cookie); //cookie
					if (--j == 0) {
						eof = false;
						break;
					}
				} while (_findnext(handle, &fileinfo) == 0);
			}

			_findclose(handle);
		}

		bFollows = false;
		Write(&bFollows);
		Write(&eof); //eof
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureREADDIRPLUS(void)
{
	std::string path;
	cookie3 cookie;
	cookieverf3 cookieverf;
	count3 dircount, maxcount;
	PostOpAttr dir_attributes;
	fileid3 fileid;
	NFSv3Filename name;
	PostOpAttr name_attributes;
	PostOpFH3 name_handle;
	bool eof;
	nfsstat3 stat;
	char filePath[MAXPATHLEN];
	int nFound;
	intptr_t handle;
	struct _finddata_t fileinfo;
	unsigned int i, j;
	bool bFollows;

	PrintLog("READDIRPLUS");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	Read(&cookie);
	Read(&cookieverf);
	Read(&dircount);
	Read(&maxcount);
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		dir_attributes.attributesFollow = GetFileAttributesForNFS(cStr, &dir_attributes.attributes);

		if (!dir_attributes.attributesFollow) {
			stat = NFS3ERR_IO;
		}
	}

	Write(&stat);
	Write(&dir_attributes);

	if (stat == NFS3_OK) {
		Write(&cookieverf);
		sprintf_s(filePath, "%s\\*", cStr);
		handle = _findfirst(filePath, &fileinfo);
		eof = true;

		if (handle) {
			nFound = 0;

			for (i = (unsigned int)cookie; i > 0; i--) {
				nFound = _findnext(handle, &fileinfo);
			}

			if (nFound == 0) {
				bFollows = true;
				j = 10;

				do {
					Write(&bFollows); //value follows
					sprintf_s(filePath, "%s\\%s", cStr, fileinfo.name);
					fileid = GetFileID(filePath);
					Write(&fileid); //file id
					name.Set(fileinfo.name);
					Write(&name); //name
					++cookie;
					Write(&cookie); //cookie
					name_attributes.attributesFollow = GetFileAttributesForNFS(filePath, &name_attributes.attributes);
					Write(&name_attributes);
					name_handle.handleFollows = GetFileHandle(filePath, &name_handle.handle);
					Write(&name_handle);

					if (--j == 0) {
						eof = false;
						break;
					}
				} while (_findnext(handle, &fileinfo) == 0);
			}

			_findclose(handle);
		}

		bFollows = false;
		Write(&bFollows); //value follows
		Write(&eof); //eof
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureFSSTAT(void)
{
	std::string path;
	PostOpAttr objAttributes;
	size3 tbytes, fbytes, abytes, tfiles, ffiles, afiles;
	uint32 invarsec;

	nfsstat3 stat;

	PrintLog("FSSTAT");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		objAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &objAttributes.attributes);

		if (objAttributes.attributesFollow
			&& GetDiskFreeSpaceEx(cStr, (PULARGE_INTEGER)&fbytes, (PULARGE_INTEGER)&tbytes, (PULARGE_INTEGER)&abytes)
			) {
			//tfiles = 99999999999;
			//ffiles = 99999999999;
			//afiles = 99999999999;
			invarsec = 0;
		}
		else {
			stat = NFS3ERR_IO;
		}
	}

	Write(&stat);
	Write(&objAttributes);

	if (stat == NFS3_OK) {
		Write(&tbytes);
		Write(&fbytes);
		Write(&abytes);
		Write(&tfiles);
		Write(&ffiles);
		Write(&afiles);
		Write(&invarsec);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureFSINFO(void)
{
	std::string path;
	PostOpAttr objAttributes;
	uint32 rtmax, rtpref, rtmult, wtmax, wtpref, wtmult, dtpref;
	size3 maxfilesize;
	NFSTime3 time_delta;
	uint32 properties;
	nfsstat3 stat;

	PrintLog("FSINFO");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		objAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &objAttributes.attributes);

		if (objAttributes.attributesFollow) {
			rtmax = 65536;
			rtpref = 32768;
			rtmult = 4096;
			wtmax = 65536;
			wtpref = 32768;
			wtmult = 4096;
			dtpref = 8192;
			maxfilesize = 0x7FFFFFFFFFFFFFFF;
			time_delta.seconds = 1;
			time_delta.nseconds = 0;
			properties = FSF3_LINK | FSF3_SYMLINK | FSF3_CANSETTIME;
		}
		else {
			stat = NFS3ERR_SERVERFAULT;
		}
	}

	Write(&stat);
	Write(&objAttributes);

	if (stat == NFS3_OK) {
		Write(&rtmax);
		Write(&rtpref);
		Write(&rtmult);
		Write(&wtmax);
		Write(&wtpref);
		Write(&wtmult);
		Write(&dtpref);
		Write(&maxfilesize);
		Write(&time_delta);
		Write(&properties);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedurePATHCONF(void)
{
	std::string path;
	PostOpAttr objAttributes;
	nfsstat3 stat;
	uint32 linkmax, name_max;
	bool no_trunc, chown_restricted, case_insensitive, case_preserving;

	PrintLog("PATHCONF");
	bool validHandle = GetPath(path);
	const char* cStr = validHandle ? path.c_str() : NULL;
	stat = CheckFile(cStr);

	if (stat == NFS3_OK) {
		objAttributes.attributesFollow = GetFileAttributesForNFS(cStr, &objAttributes.attributes);

		if (objAttributes.attributesFollow) {
			linkmax = 1023;
			name_max = 255;
			no_trunc = true;
			chown_restricted = true;
			case_insensitive = true;
			case_preserving = true;
		}
		else {
			stat = NFS3ERR_SERVERFAULT;
		}
	}

	Write(&stat);
	Write(&objAttributes);

	if (stat == NFS3_OK) {
		Write(&linkmax);
		Write(&name_max);
		Write(&no_trunc);
		Write(&chown_restricted);
		Write(&case_insensitive);
		Write(&case_preserving);
	}

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureCOMMIT(void)
{
	std::string path;
	int handleId;
	offset3 offset;
	count3 count;
	WccData file_wcc;
	nfsstat3 stat;
	NFSv3FileHandle file;
	writeverf3 verf;

	PrintLog("COMMIT");
	Read(&file);
	bool validHandle = GetFilePath(file.contents, path);
	const char* cStr = validHandle ? path.c_str() : NULL;

	if (validHandle) {
		PrintLog(" %s ", path.c_str());
	}

	// offset and count are unused
	// offset never was anything else than 0 in my tests
	// count does not matter in the way COMMIT is implemented here
	// to fulfill the spec this should be improved
	Read(&offset);
	Read(&count);

	file_wcc.before.attributesFollow = GetFileAttributesForNFS(cStr, &file_wcc.before.attributes);

	handleId = *(unsigned int*)file.contents;

	if (unstableStorageFile.count(handleId) != 0) {
		if (unstableStorageFile[handleId] != NULL) {
			fclose(unstableStorageFile[handleId]);
			unstableStorageFile.erase(handleId);
			stat = NFS3_OK;
		}
		else {
			stat = NFS3ERR_IO;
		}
	}
	else {
		stat = NFS3_OK;
	}

	file_wcc.after.attributesFollow = GetFileAttributesForNFS(cStr, &file_wcc.after.attributes);

	Write(&stat);
	Write(&file_wcc);
	// verf should be the timestamp the server startet to notice reboots
	verf = 0;
	Write(&verf);

	return stat;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::ProcedureNOIMP(void)
{
	PrintLog("NOIMP");
	m_result = PRC_NOTIMP;

	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(bool* pBool)
{
	uint32 b;

	if (m_inStream->Read(&b) < sizeof(uint32)) {
		throw __LINE__;
	}

	*pBool = b == 1;
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(uint32* pUint32)
{
	if (m_inStream->Read(pUint32) < sizeof(uint32)) {
		throw __LINE__;
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(uint64* pUint64)
{
	if (m_inStream->Read8(pUint64) < sizeof(uint64)) {
		throw __LINE__;
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(SAttr3* pAttr)
{
	Read(&pAttr->mode.setIt);

	if (pAttr->mode.setIt) {
		Read(&pAttr->mode.mode);
	}

	Read(&pAttr->uid.setIt);

	if (pAttr->uid.setIt) {
		Read(&pAttr->uid.uid);
	}

	Read(&pAttr->gid.setIt);

	if (pAttr->gid.setIt) {
		Read(&pAttr->gid.gid);
	}

	Read(&pAttr->size.setIt);

	if (pAttr->size.setIt) {
		Read(&pAttr->size.size);
	}

	Read(&pAttr->atime.setIt);

	if (pAttr->atime.setIt == SET_TO_CLIENT_TIME) {
		Read(&pAttr->atime.atime);
	}

	Read(&pAttr->mtime.setIt);

	if (pAttr->mtime.setIt == SET_TO_CLIENT_TIME) {
		Read(&pAttr->mtime.mtime);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(SAttrGuard3* pGuard)
{
	Read(&pGuard->check);

	if (pGuard->check) {
		Read(&pGuard->objCtime);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(DirOpArgs3* pDir)
{
	Read(&pDir->dir);
	Read(&pDir->name);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(Opaque* pOpaque)
{
	uint32 len, byte;

	Read(&len);
	pOpaque->SetSize(len);

	if (m_inStream->Read(pOpaque->contents, len) < len) {
		throw __LINE__;
	}

	len = 4 - (len & 3);

	if (len != 4) {
		if (m_inStream->Read(&byte, len) < len) {
			throw __LINE__;
		}
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(NFSTime3* pTime)
{
	Read(&pTime->seconds);
	Read(&pTime->nseconds);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(CreateHow3* pHow)
{
	Read(&pHow->mode);

	if (pHow->mode == UNCHECKED || pHow->mode == GUARDED) {
		Read(&pHow->objAttributes);
	}
	else {
		Read(&pHow->verf);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Read(SymlinkData3* pSymlink)
{
	Read(&pSymlink->symlinkAttributes);
	Read(&pSymlink->symlinkData);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(bool* pBool)
{
	m_outStream->Write(*pBool ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(uint32* pUint32)
{
	m_outStream->Write(*pUint32);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(uint64* pUint64)
{
	m_outStream->Write8(*pUint64);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(FAttr3* pAttr)
{
	Write(&pAttr->type);
	Write(&pAttr->mode);
	Write(&pAttr->nlink);
	Write(&pAttr->uid);
	Write(&pAttr->gid);
	Write(&pAttr->size);
	Write(&pAttr->used);
	Write(&pAttr->rdev);
	Write(&pAttr->fsid);
	Write(&pAttr->fileid);
	Write(&pAttr->atime);
	Write(&pAttr->mtime);
	Write(&pAttr->ctime);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(Opaque* pOpaque)
{
	uint32 len, byte;

	Write(&pOpaque->length);
	m_outStream->Write(pOpaque->contents, pOpaque->length);
	len = pOpaque->length & 3;

	if (len != 0) {
		byte = 0;
		m_outStream->Write(&byte, 4 - len);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(WccData* pWcc)
{
	Write(&pWcc->before);
	Write(&pWcc->after);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(PostOpAttr* pAttr)
{
	Write(&pAttr->attributesFollow);

	if (pAttr->attributesFollow) {
		Write(&pAttr->attributes);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(PreOpAttr* pAttr)
{
	Write(&pAttr->attributesFollow);

	if (pAttr->attributesFollow) {
		Write(&pAttr->attributes);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(PostOpFH3* pObj)
{
	Write(&pObj->handleFollows);

	if (pObj->handleFollows) {
		Write(&pObj->handle);
	}
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(NFSTime3* pTime)
{
	Write(&pTime->seconds);
	Write(&pTime->nseconds);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(SpecData3* pSpec)
{
	Write(&pSpec->specdata1);
	Write(&pSpec->specdata2);
}

/////////////////////////////////////////////////////////////////////
void NFS3Prog::Write(WccAttr* pAttr)
{
	Write(&pAttr->size);
	Write(&pAttr->mtime);
	Write(&pAttr->ctime);
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetPath(std::string& path)
{
	NFSv3FileHandle object;

	Read(&object);
	bool valid = GetFilePath(object.contents, path);
	if (valid) {
		PrintLog(" %s ", path.c_str());
	}
	else {
		PrintLog(" File handle is invalid ");
	}

	return valid;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::ReadDirectory(std::string& dirName, std::string& fileName)
{
	DirOpArgs3 fileRequest;
	Read(&fileRequest);

	if (GetFilePath(fileRequest.dir.contents, dirName)) {
		fileName = std::string(fileRequest.name.name);
		return true;
	}
	else {
		return false;
	}

	//PrintLog(" %s | %s ", dirName.c_str(), fileName.c_str());
}

/////////////////////////////////////////////////////////////////////
char* NFS3Prog::GetFullPath(std::string& dirName, std::string& fileName)
{
	//TODO: Return std::string
	static char fullPath[MAXPATHLEN + 1];

	if (dirName.size() + 1 + fileName.size() > MAXPATHLEN) {
		return NULL;
	}

	sprintf_s(fullPath, "%s\\%s", dirName.c_str(), fileName.c_str()); //concate path and filename
	PrintLog(" %s ", fullPath);

	return fullPath;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::CheckFile(const char* fullPath)
{
	if (fullPath == NULL) {
		return NFS3ERR_STALE;
	}

	//if (!FileExists(fullPath)) {
	if (_access(fullPath, 0) != 0) {
		return NFS3ERR_NOENT;
	}

	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
nfsstat3 NFS3Prog::CheckFile(const char* directory, const char* fullPath)
{
	// FileExists will not work for the root of a drive, e.g. \\?\D:\, therefore check if it is a drive root with GetDriveType
	if (directory == NULL || (!FileExists(directory) && GetDriveType(directory) < 2) || fullPath == NULL) {
		return NFS3ERR_STALE;
	}

	if (!FileExists(fullPath)) {
		return NFS3ERR_NOENT;
	}

	return NFS3_OK;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetFileHandle(const char* path, NFSv3FileHandle* pObject)
{
	if (!::GetFileHandle(path)) {
		PrintLog("no filehandle(path %s)", path);
		return false;
	}
	auto err = memcpy_s(pObject->contents, NFS3_FHSIZE, ::GetFileHandle(path), pObject->length);
	if (err != 0) {
		PrintLog(" err %d ", err);
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
bool NFS3Prog::GetFileAttributesForNFS(const char* path, WccAttr* pAttr)
{
	struct stat data;

	if (path == NULL) {
		return false;
	}

	if (stat(path, &data) != 0) {
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
bool NFS3Prog::GetFileAttributesForNFS(const char* path, FAttr3* pAttr)
{
	DWORD fileAttr;
	BY_HANDLE_FILE_INFORMATION lpFileInformation;
	HANDLE hFile;
	DWORD dwFlagsAndAttributes;

	fileAttr = GetFileAttributes(path);

	if (path == NULL || fileAttr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	dwFlagsAndAttributes = 0;
	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY) {
		pAttr->type = NF3DIR;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_DIRECTORY | FILE_FLAG_BACKUP_SEMANTICS;
	}
	else if (fileAttr & FILE_ATTRIBUTE_ARCHIVE) {
		pAttr->type = NF3REG;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_OVERLAPPED;
	}
	else if (fileAttr & FILE_ATTRIBUTE_NORMAL) {
		pAttr->type = NF3REG;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
	}
	else {
		pAttr->type = 0;
	}

	if (fileAttr & FILE_ATTRIBUTE_REPARSE_POINT) {
		pAttr->type = NF3LNK;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_REPARSE_POINT | FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS;
	}

	hFile = CreateFile(path, FILE_READ_EA, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

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
	LARGE_INTEGER date, adjust;
	date.HighPart = ft.dwHighDateTime;
	date.LowPart = ft.dwLowDateTime;

	// 100-nanoseconds = milliseconds * 10000
	adjust.QuadPart = 11644473600000 * 10000;

	// removes the diff between 1970 and 1601
	date.QuadPart -= adjust.QuadPart;

	// converts back from 100-nanoseconds to seconds
	return (unsigned int)(date.QuadPart / 10000000);
}