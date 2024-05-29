/////////////////////////////////////////////////////////////////////
/// file: NFS3Prog.h
///
/// summary: NFSv3 RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_NFS3PROG_H
#define ICENFSD_NFS3PROG_H

#include "RPCProg.h"

#include <string>
#include <memory>
#include <windows.h>
#include <unordered_map>

using FileId3 = uint64_t;
using Cookie3 = uint64_t;
using Uid3 = uint32_t;
using Gid3 = uint32_t;
using Size3 = uint64_t;
using Offset3 = uint64_t;
using Mode3 = uint32_t;
using Count3 = uint32_t;
using NfsStat3 = uint32_t;
using FType3 = uint32_t;
using StableHow = uint32_t;
using TimeHow = uint32_t;
using CreateMode3 = uint32_t;
using CookieVerf3 = uint64_t;
using CreateVerf3 = uint64_t;
using WriteVerf3 = uint64_t;

struct CreateHow3;
struct DirOpArgs3;
struct FAttr3;
struct NFSTime3;
struct NFSv3FileHandle;
struct Opaque;
struct PostOpAttr;
struct PostOpFH3;
struct PreOpAttr;
struct SAttr3;
struct SAttrGuard3;
struct SpecData3;
struct SymlinkData3;
struct WccAttr;
struct WccData;

class FileTable;

class NFS3Prog : public RPCProg
{
public:
	NFS3Prog(std::shared_ptr<FileTable> fileTable, unsigned int uid, unsigned int gid);
	~NFS3Prog() = default;

	int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) override;

protected:
	unsigned int m_uid, m_gid;

	NfsStat3 ProcedureNULL(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureGETATTR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureSETATTR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureLOOKUP(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureACCESS(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureREADLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureREAD(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureWRITE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureCREATE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureMKDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureSYMLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureMKNOD(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureREMOVE(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureRMDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureRENAME(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureLINK(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureREADDIR(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureREADDIRPLUS(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureFSSTAT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureFSINFO(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedurePATHCONF(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureCOMMIT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);
	NfsStat3 ProcedureNOIMP(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);

private:
	std::string GetPath(IInputStream& inStream);
	bool ReadDirectory(IInputStream& inStream, std::string& dirName, std::string& fileName);
	std::string GetFullPath(const std::string& dirName, const std::string& fileName);
	NfsStat3 CheckFile(const std::string& fullPath);
	NfsStat3 CheckFile(const std::string&, const std::string& fullPath);
	bool GetFileHandle(const std::string& path, NFSv3FileHandle* pObject);
	bool GetFileAttributesForNFS(const std::string& path, WccAttr* pAttr);
	bool GetFileAttributesForNFS(const std::string& path, FAttr3* pAttr);
	UINT32 FileTimeToPOSIX(FILETIME ft);
	std::unordered_map<int, FILE*> unstableStorageFile;

	std::shared_ptr<FileTable> m_fileTable;
};

#endif // ICENFSD_NFS3PROG_H
