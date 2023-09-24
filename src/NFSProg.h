/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_NFSPROG_H
#define ICENFSD_NFSPROG_H

#include "RPCProg.h"
#include <memory>

class NFS3Prog;

class NFSProg : public RPCProg
{
 public:
    NFSProg();
    ~NFSProg();

    void SetUserID(unsigned int uid, unsigned int gid);
    int Process(IInputStream *inStream, IOutputStream *outStream, ProcessParam *param);
    void EnableLog(bool enableLog);

private:
    unsigned int m_uid, m_gid;
    std::unique_ptr<NFS3Prog> m_nfs3;
};

#endif // ICENFSD_NFSPROG_H
