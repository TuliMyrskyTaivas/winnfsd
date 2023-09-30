/////////////////////////////////////////////////////////////////////
/// file: PortmapProg.h
///
/// summary: Port mapper RPC
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_PORTMAPPROG_H
#define ICENFSD_PORTMAPPROG_H

#include "RPCProg.h"

#define PORT_NUM 10

class PortmapProg : public RPCProg
{
public:
	PortmapProg();
	virtual ~PortmapProg() = default;

	void Set(unsigned long prog, unsigned long port);
	int Process(IInputStream* inStream, IOutputStream* outStream, ProcessParam* param);

protected:
	unsigned long m_portTable[PORT_NUM];
	IInputStream* m_inStream;
	IOutputStream* m_outStream;

	int ProcedureNOIMP() noexcept;
	int ProcedureNULL() noexcept;
	int ProcedureSET() noexcept;
	int ProcedureUNSET() noexcept;
	int ProcedureGETPORT() noexcept;
	int ProcedureDUMP() noexcept;
	int ProcedureCALLIT() noexcept;

private:
	ProcessParam* m_param;

	void Write(unsigned long prog, unsigned long vers, unsigned long proto, unsigned long port);
};

#endif // ICENFSD_PORTMAPPROG_H
