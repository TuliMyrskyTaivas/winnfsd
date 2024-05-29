/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_NFSPROG_H
#define ICENFSD_NFSPROG_H

#include "RPCProg.h"

#include <memory>

class FileTable;
class NFS3Prog;

class NFSProg : public RPCProg
{
public:
	NFSProg(std::shared_ptr<FileTable> fileTable, unsigned int uid, unsigned int gid);
	~NFSProg();

	int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) override;

private:
	std::unique_ptr<NFS3Prog> m_nfs3;
};

#endif // ICENFSD_NFSPROG_H
