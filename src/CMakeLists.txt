add_executable (icenfsd
    conv.cpp
    conv.h
    DatagramSocket.cpp
    DatagramSocket.h
    FileTable.cpp
    FileTable.h
    FileTree.cpp
    FileTree.h
    InputStream.h
    MountProg.cpp
    MountProg.h
    NFS3Prog.cpp
    NFS3Prog.h
    NFSProg.cpp
    NFSProg.h
    OutputStream.h
    PortmapProg.cpp
    PortmapProg.h
    resource.h
    RPCProg.cpp
    RPCProg.h
    RPCServer.cpp
    RPCServer.h
    ServerSocket.cpp
    ServerSocket.h
    Socket.cpp
    Socket.h
    SocketListener.h
    SocketStream.cpp
    SocketStream.h
    tree.hh
    winnfsd.cpp
    WinNFSd.rc
)

target_link_libraries(icenfsd ws2_32)