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
//    VESS Module:  vsUDPNetworkInterface.h++
//
//    Description:  Class supporting UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef UDP_NETWORK_INTERFACE_HPP
#define UDP_NETWORK_INTERFACE_HPP

#include "vsNetworkInterface.h++"

class VS_LLIO_DLL vsUDPNetworkInterface : public vsNetworkInterface
{
public:
                          vsUDPNetworkInterface(char *address, u_short port);
                          vsUDPNetworkInterface(u_short port);
    virtual               ~vsUDPNetworkInterface();

    virtual const char    *getClassName();

    int                   read(u_char *buffer, u_long len);
    int                   write(u_char *buffer, u_long len);
};

#endif

