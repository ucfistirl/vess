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
//    VESS Module:  vsClusterConfig.c++
//
//    Description:  Contains a listing of the IP addresses of machines in a
//                  cluster.
//                  
//    Author(s):    Ben Douglass
//
//------------------------------------------------------------------------

#ifndef VS_CLUSTERCONFIG_HPP
#define VS_CLUSTERCONFIG_HPP

#include "vsGlobals.h++"

class VS_SYSTEM_DLL vsClusterConfig
{
private:

    int             numOfSlaves;
    unsigned char   masterAddress[4];
    unsigned char   **slaveAddress;

    bool            legitimate;

public:

    vsClusterConfig(const char *master, const int slaveCount, 
            const char **slaves);
    ~vsClusterConfig();

    bool                    isValid();

    const unsigned char*    getMaster();
    const unsigned char*    getSlave(int slaveIndex);
    int                     numSlaves();
};
#endif
