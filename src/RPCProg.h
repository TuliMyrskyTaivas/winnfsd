/////////////////////////////////////////////////////////////////////
/// file: RPCProg.h
///
/// summary: RPC program
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_RPCPROG_H
#define ICENFSD_RPCPROG_H

#include <string>

/* The maximum number of bytes in a pathname argument. */
#define MAXPATHLEN 1024

/* The maximum number of bytes in a file name argument. */
#define MAXNAMELEN 255

/* The size in bytes of the opaque file handle. */
#define NFS_FHSIZE 32
#define NFS3_FHSIZE 64

enum
{
	PRC_OK,
	PRC_FAIL,
	PRC_NOTIMP
};

struct RPCParam
{
	unsigned int version;
	unsigned int procNum;
	std::string remoteAddr;
};

class IInputStream;
class IOutputStream;

class RPCProg
{
public:
	virtual ~RPCProg() = default;
	virtual int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param) = 0;
};

#endif // ICENFSD_RPCPROG_H
