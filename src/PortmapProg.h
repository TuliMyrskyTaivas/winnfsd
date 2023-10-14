/////////////////////////////////////////////////////////////////////
/// file: PortmapProg.h
///
/// summary: Port mapper RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_PORTMAPPROG_H
#define ICENFSD_PORTMAPPROG_H

#include "RPCProg.h"
#include <map>

class PortmapProg : public RPCProg
{
public:
	virtual ~PortmapProg() = default;

	void Set(uint32_t prog, uint32_t port);
	int Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param);

private:
	std::map<uint32_t, uint32_t> m_portTable;

	int ProcedureNULL(IInputStream& inStream, IOutputStream& outStream) noexcept;
	int ProcedureSET(IInputStream& inStream, IOutputStream& outStream) noexcept;
	int ProcedureUNSET(IInputStream& inStream, IOutputStream& outStream) noexcept;
	int ProcedureGETPORT(IInputStream& inStream, IOutputStream& outStream) noexcept;
	int ProcedureDUMP(IInputStream& inStream, IOutputStream& outStream) noexcept;
	int ProcedureCALLIT(IInputStream& inStream, IOutputStream& outStream) noexcept;
};

#endif // ICENFSD_PORTMAPPROG_H
