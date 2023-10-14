/////////////////////////////////////////////////////////////////////
/// file: MountProg.h
///
/// summary: NFS mount RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_MOUNTPROG_H
#define ICENFSD_MOUNTPROG_H

#include "RPCProg.h"
#include <vector>
#include <string>
#include <map>

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
	virtual ~MountProg() = default;

	void Export(const std::string& path, const std::string& alias);
	int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) override;

protected:
	std::string m_pathFile;
	std::map<std::string, std::string> m_pathMap;
	std::vector<std::string> m_clients;

	int ProcedureNULL(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;
	int ProcedureMNT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;
	int ProcedureUMNT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;
	int ProcedureUMNTALL(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;
	int ProcedureEXPORT(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;
	int ProcedureNOIMP(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) noexcept;

	std::string FormatPath(const std::string& path, PathFormat format) const;

private:
	std::string GetPath(IInputStream& inStreams);
	char* GetPath(int& pathNumber);
};

#endif // ICENFSD_MOUNTPROG_H
