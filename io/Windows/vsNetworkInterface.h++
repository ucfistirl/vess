
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
//    VESS Module:  vsNetworkInterface.h++
//
//    Description:  Class supporting network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef NETWORK_INTERFACE_HPP
#define NETWORK_INTERFACE_HPP

#include "vsIPCInterface.h++"


class VS_IO_DLL vsNetworkInterface : public vsIPCInterface
{
protected:

    SOCKET                socketValue;

    struct sockaddr_in    readName;
    int                   readNameLength;
    struct sockaddr_in    writeName;
    int                   writeNameLength;

public:

                          vsNetworkInterface();
    virtual               ~vsNetworkInterface();

    virtual const char    *getClassName();

    virtual void          enableBlocking();
    virtual void          disableBlocking();
};

#endif
