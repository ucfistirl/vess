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
//    VESS Module:  vsIPList.c++
//
//    Description:  Contains a listing of the IP addresses of machines in a
//                  cluster.
//                  
//
//    Author(s):    Ben Douglass
//
//------------------------------------------------------------------------

#include "vsClusterConfig.h++"
#include <netdb.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

vsClusterConfig::vsClusterConfig(const char *master, const int slaveCount, 
        const char **slaves)
{
    struct hostent *ip;
    int i;
    
    legitimate = 0;
    ip = gethostbyname(master);
    if(!ip || !ip->h_addr_list[0])
    {
        printf("vsClusterConfig::vsClusterConfig: Can't locate master\n");
        slaveAddress = NULL;
        return;
    }
    memcpy(masterAddress,ip->h_addr_list[0],4*sizeof(char));
    
    numOfSlaves = slaveCount;
    slaveAddress = (unsigned char**)calloc( slaveCount, sizeof(char*) );
    

    for(i=0; i<slaveCount; i++)
    {

        ip = gethostbyname(slaves[i]);

        if(!ip || !ip->h_addr_list[0])
        {
            printf("vsClusterConfig::vsClusterConfig: Can't locate slave %s\n",
                    slaves[i]);
            slaveAddress[i] = NULL;
            return;
        }

        slaveAddress[i] = (unsigned char*)calloc(4,sizeof(char));
        memcpy(slaveAddress[i],ip->h_addr_list[0],4);

    }
    legitimate = 1;
}

vsClusterConfig::~vsClusterConfig(void)
{
    int i;
    if(slaveAddress)
    {
        for(i=0;i<numOfSlaves && slaveAddress[i];i++)
        {
            free(slaveAddress[i]);
        }

        free(slaveAddress);
    }
}

bool vsClusterConfig::isValid(void)
{
    return legitimate;
}

const unsigned char* vsClusterConfig::getMaster(void)
{
    return masterAddress;
}

const unsigned char* vsClusterConfig::getSlave(int slaveIndex)
{
    if(slaveIndex >=0 && slaveIndex < numOfSlaves)
    {
        return slaveAddress[slaveIndex];
    }
    return NULL;
}

int vsClusterConfig::numSlaves(void)
{
    return numOfSlaves;
}
