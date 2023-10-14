/////////////////////////////////////////////////////////////////////
/// file: PortmapProg.cpp
///
/// summary: Port mapper RPC
/////////////////////////////////////////////////////////////////////

#include "PortmapProg.h"
#include "InputStream.h"
#include "OutputStream.h"

#include <boost/log/trivial.hpp>
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

struct PortmapHeader
{
	uint32_t prog;
	uint32_t vers;
	uint32_t proto;
	uint32_t port;
};

/////////////////////////////////////////////////////////////////////
void PortmapProg::Set(uint32_t prog, uint32_t port)
{
	m_portTable[prog] = port;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	typedef int (PortmapProg::* PPROC)(IInputStream&, IOutputStream&);
	static PPROC pf[] = {
		&PortmapProg::ProcedureNULL, &PortmapProg::ProcedureSET, &PortmapProg::ProcedureUNSET,
		&PortmapProg::ProcedureGETPORT, &PortmapProg::ProcedureDUMP, &PortmapProg::ProcedureCALLIT
	};

	if (param.procNum >= sizeof(pf) / sizeof(PPROC))
	{
		BOOST_LOG_TRIVIAL(debug) << "PORTMAP: procedure " << param.procNum << " not implemented";
		return PRC_NOTIMP;
	}

	return (this->*pf[param.procNum])(inStream, outStream);
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureNULL(IInputStream&, IOutputStream&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: NULL command";
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureSET(IInputStream&, IOutputStream&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: SET command (not implemented)";
	return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureUNSET(IInputStream&, IOutputStream&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: UNSET command (not implemented)";
	return PRC_NOTIMP;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureGETPORT(IInputStream& input, IOutputStream& output) noexcept
{
	PortmapHeader header{};
	unsigned long port = 0;

	input.Read(&header.prog);  //program
	input.Skip(12);
	port = m_portTable[header.prog];
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: GETPORT command: program " << header.prog << " listens port " << port;
	output.Write(port);  //port
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureDUMP(IInputStream&, IOutputStream& outStream) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: DUMP command";

	auto Write = [&outStream](unsigned long prog, unsigned long vers, unsigned long proto, unsigned long port)->void {
		outStream.Write(1);
		outStream.Write(prog);
		outStream.Write(vers);
		outStream.Write(proto);
		outStream.Write(port);
	};

	Write(PROG_PORTMAP, 2, IPPROTO_TCP, PORTMAP_PORT);
	Write(PROG_PORTMAP, 2, IPPROTO_UDP, PORTMAP_PORT);
	Write(PROG_NFS, 3, IPPROTO_TCP, NFS_PORT);
	Write(PROG_NFS, 3, IPPROTO_UDP, NFS_PORT);
	Write(PROG_MOUNT, 3, IPPROTO_TCP, MOUNT_PORT);
	Write(PROG_MOUNT, 3, IPPROTO_UDP, MOUNT_PORT);

	outStream.Write(0);
	return PRC_OK;
}

/////////////////////////////////////////////////////////////////////
int PortmapProg::ProcedureCALLIT(IInputStream&, IOutputStream&) noexcept
{
	BOOST_LOG_TRIVIAL(debug) << "PORTMAP: CALLIT command (not implemented)";
	return PRC_NOTIMP;
}