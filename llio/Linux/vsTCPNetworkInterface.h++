
//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2003, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsTCPNetworkInterface.h++
//
//    Description:  Class supporting TCP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef TCP_NETWORK_INTERFACE_HPP
#define TCP_NETWORK_INTERFACE_HPP

#include "vsNetworkInterface.h++"

#define VS_MAX_TCP_CLIENTS   20

class vsTCPNetworkInterface : public vsNetworkInterface
{
protected:

    int                   clientSockets[VS_MAX_TCP_CLIENTS];
    struct sockaddr_in    clientNames[VS_MAX_TCP_CLIENTS];
    socklen_t             clientNameLengths[VS_MAX_TCP_CLIENTS];
    int                   numClientSockets;

public:

                vsTCPNetworkInterface(char *address, u_short port);
                vsTCPNetworkInterface(u_short port);
    virtual     ~vsTCPNetworkInterface();

    virtual const char    *getClassName();

    void        allowConnections(int backlog);
    int         acceptConnection();
    void        enableBlockingOnClient(int clientID);
    void        disableBlockingOnClient(int clientID);

    int         makeConnection();

    int         read(u_char *buffer, u_long len);
    int         read(int clientID, u_char *buffer, u_long len);
    int         write(u_char *buffer, u_long len);
    int         write(int clientID, u_char *buffer, u_long len);
};

#endif

