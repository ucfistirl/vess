//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsUDPUnicastNetworkInterface.h++
//
//    Description:  Class supporting unicast UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef VS_UDP_UNICAST_NETWORK_INTERFACE_HPP
#define VS_UDP_UNICAST_NETWORK_INTERFACE_HPP


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "vsUDPNetworkInterface.h++"


class vsUDPUnicastNetworkInterface : public vsUDPNetworkInterface
{
public:
                vsUDPUnicastNetworkInterface(char *address, short port, 
                                             int blocking);
    virtual     ~vsUDPUnicastNetworkInterface();

    virtual const char    *getClassName();
};

#endif

