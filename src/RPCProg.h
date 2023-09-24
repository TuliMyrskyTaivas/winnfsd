/////////////////////////////////////////////////////////////////////
/// file: RPCProg.h
///
/// summary: RPC program
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_RPCPROG_H
#define ICENFSD_RPCPROG_H

/* The maximum number of bytes in a pathname argument. */
#define MAXPATHLEN 1024

/* The maximum number of bytes in a file name argument. */
#define MAXNAMELEN 255

/* The size in bytes of the opaque file handle. */
#define FHSIZE 32
#define NFS3_FHSIZE 64

enum
{
    PRC_OK,
    PRC_FAIL,
    PRC_NOTIMP
};

typedef struct
{
    unsigned int nVersion;
    unsigned int nProc;
    char *pRemoteAddr;
} ProcessParam;

class IInputStream;
class IOutputStream;

class RPCProg
{
public:
    RPCProg();
    virtual ~RPCProg() = default;

    virtual int Process(IInputStream *inStream, IOutputStream *outStream, ProcessParam *param) = 0;
    virtual void EnableLog(bool enableLog);

protected:
    bool m_enableLog;
    virtual int PrintLog(const char *format, ...);
};

#endif // ICENFSD_RPCPROG_H
