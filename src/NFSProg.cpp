/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#include "NFSProg.h"
#include "NFS3Prog.h"

#include <boost/log/trivial.hpp>

/////////////////////////////////////////////////////////////////////
NFSProg::NFSProg(std::shared_ptr<FileTable> fileTable, unsigned int uid, unsigned int gid)
	: RPCProg()
	, m_nfs3(std::make_unique<NFS3Prog>(fileTable, uid, gid))
{}

/////////////////////////////////////////////////////////////////////
NFSProg::~NFSProg()
{}

/////////////////////////////////////////////////////////////////////
int NFSProg::Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	if (param.version == 3)
	{
		return m_nfs3->Process(inStream, outStream, param);
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Client " << param.remoteAddr << " requested NFS version "
			<< param.version << " which is not supported";
		return PRC_NOTIMP;
	}
}