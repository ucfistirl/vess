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
//    VESS Module:  IPCInterface.h++
//
//    Description:  Class supporting abstract communication
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef VS_IPC_INTERFACE_HPP
#define VS_IPC_INTERFACE_HPP

#include <sys/types.h>

class vsIPCInterface
{
public:

                   vsIPCInterface();
    virtual        ~vsIPCInterface();

    virtual int    read(u_char *buffer, u_long length) = 0;
    virtual int    write(u_char *buffer, u_long length) = 0;
};

#endif

