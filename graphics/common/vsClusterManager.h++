// ------------------------------------------------------------------------
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
// ------------------------------------------------------------------------
//
//    VESS Module:  vsClusterManager.c++
//
//    Description:  Contains a listing of the IP addresses of machines in a
//                  cluster.
//                  
//    Author(s):    Ben Douglass
//
// ------------------------------------------------------------------------

#ifndef VS_CLUSTERMANAGER_HPP
#define VS_CLUSTERMANAGER_HPP

#include "vsTCPNetworkInterface.h++"

class vsClusterManager
{
private:

    int                     numOfSlaves;
    vsTCPNetworkInterface   **slaves;

    bool                    legitimate;
    int                     stackDepth; 
   
VS_INTERNAL:

    // Functions for keeping track of calls generated internally by VESS
    int                             getStackDepth();
    void                            stackIncrement();
    void                            stackDecrement();

public:

    static vsClusterManager         *clusterManagerObject;    

                                    vsClusterManager(const int slaveCount, 
                                            const char **newSlaves);
                                    ~vsClusterManager();

    bool                            isValid();

    const vsTCPNetworkInterface     *getSlave(int slaveIndex);
    int                             numSlaves();

    void                            transmit(char *commStr);
    void                            sync();
};
#endif
