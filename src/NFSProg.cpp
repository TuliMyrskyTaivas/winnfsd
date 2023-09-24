/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#include "NFSProg.h"
#include "NFS3Prog.h"

/////////////////////////////////////////////////////////////////////
NFSProg::NFSProg()
    : RPCProg()
    , m_uid(0)
    , m_gid(0)
    , m_nfs3(nullptr)
{}

/////////////////////////////////////////////////////////////////////
NFSProg::~NFSProg()
{}

/////////////////////////////////////////////////////////////////////
void NFSProg::SetUserID(unsigned int uid, unsigned int gid)
{
    m_uid = uid;
    m_gid = gid;
}

/////////////////////////////////////////////////////////////////////
int NFSProg::Process(IInputStream *inStream, IOutputStream *outStream, ProcessParam *param)
{
    if (param->nVersion == 3)
    {
        if (!m_nfs3)
        {
            m_nfs3 = std::make_unique<NFS3Prog>(m_uid, m_gid, m_enableLog);
        }

        return m_nfs3->Process(inStream, outStream, param);
    }
    else
    {
        PrintLog("Client requested NFS version %u which isn't supported.\n", param->nVersion);
        return PRC_NOTIMP;
    }
}

/////////////////////////////////////////////////////////////////////
void NFSProg::EnableLog(bool enableLog)
{
    RPCProg::EnableLog(enableLog);

    if (m_nfs3 != nullptr)
    {
        m_nfs3->EnableLog(enableLog);
    }
}
