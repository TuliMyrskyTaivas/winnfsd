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

typedef unsigned __int64 uint64;
typedef unsigned long uint32;
typedef long int32;
typedef uint64 fileid3;
typedef uint64 cookie3;
typedef uint32 uid3;
typedef uint32 gid3;
typedef uint64 size3;
typedef uint64 offset3;
typedef uint32 mode3;
typedef uint32 count3;
typedef uint32 nfsstat3;
typedef uint32 ftype3;
typedef uint32 stable_how;
typedef uint32 time_how;
typedef uint32 createmode3;
typedef uint64 cookieverf3;
typedef uint64 createverf3;
typedef uint64 writeverf3;

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

	nfsstat3 ProcedureNULL();
	nfsstat3 ProcedureGETATTR();
	nfsstat3 ProcedureSETATTR();
	nfsstat3 ProcedureLOOKUP();
	nfsstat3 ProcedureACCESS();
	nfsstat3 ProcedureREADLINK();
	nfsstat3 ProcedureREAD();
	nfsstat3 ProcedureWRITE();
	nfsstat3 ProcedureCREATE();
	nfsstat3 ProcedureMKDIR();
	nfsstat3 ProcedureSYMLINK();
	nfsstat3 ProcedureMKNOD();
	nfsstat3 ProcedureREMOVE();
	nfsstat3 ProcedureRMDIR();
	nfsstat3 ProcedureRENAME();
	nfsstat3 ProcedureLINK();
	nfsstat3 ProcedureREADDIR();
	nfsstat3 ProcedureREADDIRPLUS();
	nfsstat3 ProcedureFSSTAT();
	nfsstat3 ProcedureFSINFO();
	nfsstat3 ProcedurePATHCONF();
	nfsstat3 ProcedureCOMMIT();
	nfsstat3 ProcedureNOIMP();

	void Read(bool* pBool);
	void Read(uint32* pUint32);
	void Read(uint64* pUint64);
	void Read(SAttr3* pAttr);
	void Read(SAttrGuard3* pGuard);
	void Read(DirOpArgs3* pDir);
	void Read(Opaque* pOpaque);
	void Read(NFSTime3* pTime);
	void Read(CreateHow3* pHow);
	void Read(SymlinkData3* pSymlink);
	void Write(bool* pBool);
	void Write(uint32* pUint32);
	void Write(uint64* pUint64);
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
	nfsstat3 CheckFile(const char* fullPath);
	nfsstat3 CheckFile(const char* directory, const char* fullPath);
	bool GetFileHandle(const char* path, NFSv3FileHandle* pObject);
	bool GetFileAttributesForNFS(const char* path, WccAttr* pAttr);
	bool GetFileAttributesForNFS(const char* path, FAttr3* pAttr);
	UINT32 FileTimeToPOSIX(FILETIME ft);
	std::unordered_map<int, FILE*> unstableStorageFile;
};

#endif // ICENFSD_NFS3PROG_H
