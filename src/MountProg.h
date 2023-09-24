/////////////////////////////////////////////////////////////////////
/// file: MountProg.h
///
/// summary: NFS mount RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_MOUNTPROG_H
#define ICENFSD_MOUNTPROG_H

#include "RPCProg.h"
#include <map>
#include <string>

#define MOUNT_NUM_MAX 100
#define MOUNT_PATH_MAX 100

enum PathFormat
{
	FORMAT_PATH = 1,
	FORMAT_PATHALIAS = 2
};

class MountProg : public RPCProg
{
 public:
    MountProg();
    virtual ~MountProg();

	bool SetPathFile(const char *file);
    void Export(const char *path, const char *pathAlias);
	bool Refresh();
    char *GetClientAddr(int index);
    int GetMountNumber() const noexcept;
    int Process(IInputStream *inStream, IOutputStream *outStream, ProcessParam *param);
	char *FormatPath(const char *path, PathFormat format);

 protected:
    int m_mountNum;
	char *m_pathFile;
	std::map<std::string, std::string> m_pathMap;
    char *m_clientAddr[MOUNT_NUM_MAX];
    IInputStream *m_inStream;
    IOutputStream *m_outStream;

    int ProcedureNULL() noexcept;
    int ProcedureMNT() noexcept;
    int ProcedureUMNT() noexcept;
    int ProcedureUMNTALL() noexcept;
    int ProcedureEXPORT() noexcept;
    int ProcedureNOIMP() noexcept;

private:
    ProcessParam *m_param;

	bool GetPath(char **returnPath);
    char *GetPath(int &pathNumber);
	bool ReadPathsFromFile(const char* fileName);
};

#endif // ICENFSD_MOUNTPROG_H
