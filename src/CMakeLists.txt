include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})
add_compile_definitions (BOOST_USE_WINAPI_VERSION=0x0600)

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
    RPCProg.h
    RPCServer.cpp
    RPCServer.h
    ServerSocket.cpp
    ServerSocket.h
    Settings.cpp
    Settings.h
    Socket.cpp
    Socket.h
    SocketListener.h
    SocketStream.cpp
    SocketStream.h
    tree.hh
    winnfsd.cpp
    WinNFSd.rc
)

target_link_libraries(icenfsd
    ws2_32
    ${Boost_LIBRARIES}
)