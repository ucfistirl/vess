
#ifndef VS_REMOTE_INTERFACE_HPP
#define VS_REMOTE_INTERFACE_HPP

#include <sys/types.h>
#include "atTCPNetworkInterface.h++"
#include "vsRemoteInterfaceBuffer.h++"

#define VS_RI_DEFAULT_CONTROL_PORT     32816

#define VS_RI_MAX_CONNECTIONS   10

class VS_SYSTEM_DLL vsRemoteInterface
{
protected:
    atTCPNetworkInterface     *tcpInterface;

    u_long                    numClients;
    int                       tcpClientIDs[VS_RI_MAX_CONNECTIONS];
    vsRemoteInterfaceBuffer   *tcpClientBuffers[VS_RI_MAX_CONNECTIONS];

public:
    vsRemoteInterface();
    vsRemoteInterface(short port);
    virtual ~vsRemoteInterface();

    void   update();
    void   send(u_char *buffer, u_long bufferLen);
};

#endif

