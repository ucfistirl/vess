
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

#include <netinet/in.h>
#include "vsIPCInterface.h++"


class vsNetworkInterface : public vsIPCInterface
{
protected:

    int                   socketValue;

    struct sockaddr_in    readName;
    socklen_t             readNameLength;
    struct sockaddr_in    writeName;
    socklen_t             writeNameLength;

public:

                    vsNetworkInterface();
    virtual         ~vsNetworkInterface();

    virtual void    enableBlocking();
    virtual void    disableBlocking();
};

#endif

