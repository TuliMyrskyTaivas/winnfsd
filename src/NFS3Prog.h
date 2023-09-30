/////////////////////////////////////////////////////////////////////
/// file: NFS3Prog.h
///
/// summary: NFSv3 RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_NFS3PROG_H
#define ICENFSD_NFS3PROG_H

#include "RPCProg.h"

#include <string>
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

class NFS3Prog : public RPCProg
{
public:
	NFS3Prog(unsigned int uid, unsigned int gid, bool enableLog);
	~NFS3Prog() = default;

	int Process(IInputStream* inStream, IOutputStream* outStream, ProcessParam* param);

protected:
	unsigned long m_uid, m_gid;
	IInputStream* m_inStream;
	IOutputStream* m_outStream;
	ProcessParam* m_param;

	NfsStat3 ProcedureNULL();
	NfsStat3 ProcedureGETATTR();
	NfsStat3 ProcedureSETATTR();
	NfsStat3 ProcedureLOOKUP();
	NfsStat3 ProcedureACCESS();
	NfsStat3 ProcedureREADLINK();
	NfsStat3 ProcedureREAD();
	NfsStat3 ProcedureWRITE();
	NfsStat3 ProcedureCREATE();
	NfsStat3 ProcedureMKDIR();
	NfsStat3 ProcedureSYMLINK();
	NfsStat3 ProcedureMKNOD();
	NfsStat3 ProcedureREMOVE();
	NfsStat3 ProcedureRMDIR();
	NfsStat3 ProcedureRENAME();
	NfsStat3 ProcedureLINK();
	NfsStat3 ProcedureREADDIR();
	NfsStat3 ProcedureREADDIRPLUS();
	NfsStat3 ProcedureFSSTAT();
	NfsStat3 ProcedureFSINFO();
	NfsStat3 ProcedurePATHCONF();
	NfsStat3 ProcedureCOMMIT();
	NfsStat3 ProcedureNOIMP();

	void Read(bool* pBool);
	void Read(uint32_t* pUint32);
	void Read(uint64_t* pUint64);
	void Read(SAttr3* pAttr);
	void Read(SAttrGuard3* pGuard);
	void Read(DirOpArgs3* pDir);
	void Read(Opaque* pOpaque);
	void Read(NFSTime3* pTime);
	void Read(CreateHow3* pHow);
	void Read(SymlinkData3* pSymlink);
	void Write(bool* pBool);
	void Write(uint32_t* pUint32);
	void Write(uint64_t* pUint64);
	void Write(FAttr3* pAttr);
	void Write(Opaque* pOpaque);
	void Write(WccData* pWcc);
	void Write(PostOpAttr* pAttr);
	void Write(PreOpAttr* pAttr);
	void Write(PostOpFH3* pObj);
	void Write(NFSTime3* pTime);
	void Write(SpecData3* pSpec);
	void Write(WccAttr* pAttr);

private:
	int m_result;

	bool GetPath(std::string& path);
	bool ReadDirectory(std::string& dirName, std::string& fileName);
	char* GetFullPath(std::string& dirName, std::string& fileName);
	NfsStat3 CheckFile(const char* fullPath);
	NfsStat3 CheckFile(const char* directory, const char* fullPath);
	bool GetFileHandle(const char* path, NFSv3FileHandle* pObject);
	bool GetFileAttributesForNFS(const char* path, WccAttr* pAttr);
	bool GetFileAttributesForNFS(const char* path, FAttr3* pAttr);
	UINT32 FileTimeToPOSIX(FILETIME ft);
	std::unordered_map<int, FILE*> unstableStorageFile;
};

#endif // ICENFSD_NFS3PROG_H
