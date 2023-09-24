/////////////////////////////////////////////////////////////////////
/// file: RPCProg.h
///
/// summary: RPC program
/////////////////////////////////////////////////////////////////////

#include "RPCProg.h"
#include "InputStream.h"
#include "OutputStream.h"

#include <cstdarg>
#include <cstdio>

/////////////////////////////////////////////////////////////////////
RPCProg::RPCProg()
    : m_enableLog(true)
{}

/////////////////////////////////////////////////////////////////////
void RPCProg::EnableLog(bool enableLog)
{
    m_enableLog = enableLog;
}

/////////////////////////////////////////////////////////////////////
int RPCProg::PrintLog(const char *format, ...)
{
    int result = 0;

    if (m_enableLog)
    {
        va_list vargs;
        va_start(vargs, format);
        result = vprintf(format, vargs);
        va_end(vargs);
    }

    return result;
}
