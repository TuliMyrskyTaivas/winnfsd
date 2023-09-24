/////////////////////////////////////////////////////////////////////
/// file: RPCServer.h
///
/// summary: RPC server implementation
/////////////////////////////////////////////////////////////////////

#include "RPCServer.h"
#include "ServerSocket.h"
#include <stdio.h>

#define MIN_PROG_NUM 100000
enum
{
    CALL = 0,
    REPLY = 1
};

enum
{
    MSG_ACCEPTED = 0,
    MSG_DENIED = 1
};

enum
{
    SUCCESS = 0,
    PROG_UNAVAIL = 1,
    PROG_MISMATCH = 2,
    PROC_UNAVAIL = 3,
    GARBAGE_ARGS = 4
};

typedef struct
{
    unsigned long flavor;
    unsigned long length;
} OPAQUE_AUTH;

typedef struct
{
    unsigned long header;
    unsigned long XID;
    unsigned long msg;
    unsigned long rpcvers;
    unsigned long prog;
    unsigned long vers;
    unsigned long proc;
    OPAQUE_AUTH cred;
    OPAQUE_AUTH verf;
} RPC_HEADER;

/////////////////////////////////////////////////////////////////////
RPCServer::RPCServer()
    : m_mutex(CreateMutex(nullptr, false, nullptr))
{
    for (int i = 0; i < PROG_NUM; i++)
    {
        m_progTable[i] = nullptr;
    }   
}

/////////////////////////////////////////////////////////////////////
RPCServer::~RPCServer()
{
    CloseHandle(m_mutex);
}

/////////////////////////////////////////////////////////////////////
void RPCServer::Set(int progNum, RPCProg *progHandle)
{
    m_progTable[progNum - MIN_PROG_NUM] = progHandle;  //set program handler
}

/////////////////////////////////////////////////////////////////////
void RPCServer::EnableLog(bool enableLog)
{
    for (int i = 0; i < PROG_NUM; i++)
    {
        if (m_progTable[i] != NULL)
        {
            m_progTable[i]->EnableLog(enableLog);
        }
    }  
}

/////////////////////////////////////////////////////////////////////
void RPCServer::SocketReceived(Socket *socket)
{
    IInputStream *inStream;
    int result;

    WaitForSingleObject(m_mutex, INFINITE);
    inStream = socket->GetInputStream();

    while (inStream->GetSize() > 0)
    {
        result = Process(socket->GetType(), inStream, socket->GetOutputStream(), socket->GetRemoteAddress());  //process input data
        socket->Send();  //send response

        if (result != PRC_OK || socket->GetType() == SOCK_DGRAM)
        {
            break;
        }
    }

    ReleaseMutex(m_mutex);
}

/////////////////////////////////////////////////////////////////////
int RPCServer::Process(int type, IInputStream *inStream, IOutputStream *outStream, char *remoteAddr)
{
    RPC_HEADER header;
    int pos = 0, size = 0;
    ProcessParam param;
    int result = PRC_OK;

    if (type == SOCK_STREAM)
    {
        inStream->Read(&header.header);
    }    

    inStream->Read(&header.XID);
    inStream->Read(&header.msg);
    inStream->Read(&header.rpcvers);  //rpc version
    inStream->Read(&header.prog);  //program
    inStream->Read(&header.vers);  //program version
    inStream->Read(&header.proc);  //procedure
    inStream->Read(&header.cred.flavor);
    inStream->Read(&header.cred.length);
    inStream->Skip(header.cred.length);
    inStream->Read(&header.verf.flavor);  //verifier

    if (inStream->Read(&header.verf.length) < sizeof(header.verf.length))
    {
        result = PRC_FAIL;
    }
        
    if (inStream->Skip(header.verf.length) < header.verf.length)
    {
        result = PRC_FAIL;
    }      

    if (type == SOCK_STREAM)
    {
        pos = outStream->GetPosition();  //remember current position
        outStream->Write(header.header);  //this value will be updated later
    }

    outStream->Write(header.XID);
    outStream->Write(REPLY);
    outStream->Write(MSG_ACCEPTED);
    outStream->Write(header.verf.flavor);
    outStream->Write(header.verf.length);

    if (result == PRC_FAIL) //input data is truncated
    { 
        outStream->Write(GARBAGE_ARGS);
    }
    else if (header.prog < MIN_PROG_NUM || header.prog >= MIN_PROG_NUM + PROG_NUM) //program is unavailable
    {
        outStream->Write(PROG_UNAVAIL);
    }
    else if (m_progTable[header.prog - MIN_PROG_NUM] == NULL) //program is unavailable
    {
        outStream->Write(PROG_UNAVAIL);
    }
    else
    {
        outStream->Write(SUCCESS);  //this value may be modified later if process failed
        param.nVersion = header.vers;
        param.nProc = header.proc;
        param.pRemoteAddr = remoteAddr;
        result = m_progTable[header.prog - MIN_PROG_NUM]->Process(inStream, outStream, &param);  //process rest input data by program

        if (result == PRC_NOTIMP) //procedure is not implemented
        {
            outStream->Seek(-4, SEEK_CUR);
            outStream->Write(PROC_UNAVAIL);
        }
        else if (result == PRC_FAIL) //input data is truncated
        {
            outStream->Seek(-4, SEEK_CUR);
            outStream->Write(GARBAGE_ARGS);
        }
    }

    if (type == SOCK_STREAM)
    {
        size = outStream->GetPosition();  //remember current position
        outStream->Seek(pos, SEEK_SET);  //seek to the position of head
        header.header = 0x80000000 + size - pos - 4;  //size of output data
        outStream->Write(header.header);  //update header
    }

    return result;
}
