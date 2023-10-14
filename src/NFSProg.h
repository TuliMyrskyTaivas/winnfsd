/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_NFSPROG_H
#define ICENFSD_NFSPROG_H

#include "RPCProg.h"

class NFSProg : public RPCProg
{
public:
	NFSProg();
	~NFSProg();

	void SetUserID(unsigned int uid, unsigned int gid);
	int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) override;

private:
	unsigned int m_uid, m_gid;
};

#endif // ICENFSD_NFSPROG_H
