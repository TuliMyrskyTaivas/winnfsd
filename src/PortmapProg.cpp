/////////////////////////////////////////////////////////////////////
/// file: PortmapProg.cpp
///
/// summary: Port mapper RPC
/////////////////////////////////////////////////////////////////////

#include "PortmapProg.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <cstring>

#define MIN_PROG_NUM 100000
enum
{
    MAPPROC_NULL = 0,
    MAPPROC_SET = 1,
    MAPPROC_UNSET = 2,
    MAPPROC_GETPORT = 3,
    MAPPROC_DUMP = 4,
    MAPPROC_CALLIT = 5
};

enum
{
    IPPROTO_TCP = 6,
    IPPROTO_UDP = 17
};

enum
{
    PORTMAP_PORT = 111,
    MOUNT_PORT = 1058,
    NFS_PORT = 2049
};

enum
{
    PROG_PORTMAP = 100000,
    PROG_NFS = 100003,
    PROG_MOUNT = 100005
};

typedef int (PortmapProg::*PPROC)();

/////////////////////////////////////////////////////////////////////
PortmapProg::PortmapProg()
    : RPCProg()
    , m_inStream(nullptr)
    , m_outStream(nullptr)
    , m_param(nullptr)
{
    memset(m_portTable, 0, PORT_NUM * sizeof(int));
}

/////////////////////////////////////////////////////////////////////
void PortmapProg::Set(unsigned long prog, unsigned long port)
{
    m_portTable[prog - MIN_PROG_NUM] = port;  //set port for program
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::Process(IInputStream *pInStream, IOutputStream *pOutStream, ProcessParam *pParam)
{
    static PPROC pf[] = {
        &PortmapProg::ProcedureNULL, &PortmapProg::ProcedureSET, &PortmapProg::ProcedureUNSET,
        &PortmapProg::ProcedureGETPORT, &PortmapProg::ProcedureDUMP, &PortmapProg::ProcedureCALLIT
    };

    PrintLog("PORTMAP ");

    if (pParam->nProc >= sizeof(pf) / sizeof(PPROC))
    {
        ProcedureNOIMP();
        PrintLog("\n");
        return PRC_NOTIMP;
    }

    m_inStream = pInStream;
    m_outStream = pOutStream;
    m_param = pParam;
    return (this->*pf[pParam->nProc])();
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureNOIMP() noexcept
{
    PrintLog("NOIMP");
    return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureNULL() noexcept
{
    PrintLog("NULL");
    return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureSET() noexcept
{
    PrintLog("SET - NOIMP");
    return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureUNSET() noexcept
{
    PrintLog("UNSET - NOIMP");
    return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureGETPORT() noexcept
{
    PORTMAP_HEADER header;
    unsigned long port;

    PrintLog("GETPORT");
    m_inStream->Read(&header.prog);  //program
    m_inStream->Skip(12);
    port = header.prog >= MIN_PROG_NUM && header.prog < MIN_PROG_NUM + PORT_NUM ? m_portTable[header.prog - MIN_PROG_NUM] : 0;
    PrintLog(" %d %d", header.prog, port);
    m_outStream->Write(port);  //port
    return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureDUMP() noexcept
{
    PrintLog("DUMP");

    Write(PROG_PORTMAP, 2, IPPROTO_TCP, PORTMAP_PORT);
    Write(PROG_PORTMAP, 2, IPPROTO_UDP, PORTMAP_PORT);
    Write(PROG_NFS, 3, IPPROTO_TCP, NFS_PORT);
    Write(PROG_NFS, 3, IPPROTO_UDP, NFS_PORT);
    Write(PROG_MOUNT, 3, IPPROTO_TCP, MOUNT_PORT);
    Write(PROG_MOUNT, 3, IPPROTO_UDP, MOUNT_PORT);

    m_outStream->Write(0);
    return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureCALLIT() noexcept
{
    PrintLog("CALLIT - NOIMP");
    return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
void PortmapProg::Write(unsigned long prog, unsigned long vers, unsigned long proto, unsigned long port)
{
    m_outStream->Write(1);
    m_outStream->Write(prog);
    m_outStream->Write(vers);
    m_outStream->Write(proto);
    m_outStream->Write(port);
}