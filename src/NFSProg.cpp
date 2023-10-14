/////////////////////////////////////////////////////////////////////
/// file: NFSProg.h
///
/// summary: NFS RPC
/////////////////////////////////////////////////////////////////////

#include "NFSProg.h"
#include "NFS3Prog.h"

#include <boost/log/trivial.hpp>

/////////////////////////////////////////////////////////////////////
NFSProg::NFSProg()
	: RPCProg()
	, m_uid(0)
	, m_gid(0)
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
int NFSProg::Process(IInputStream& inStream, IOutputStream& outStream, RPCParam& param)
{
	if (param.version == 3)
	{
		NFS3Prog nfs3(m_uid, m_gid);
		return nfs3.Process(inStream, outStream, param);
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Client " << param.remoteAddr << " requested NFS version "
			<< param.version << " which is not supported";
		return PRC_NOTIMP;
	}
}