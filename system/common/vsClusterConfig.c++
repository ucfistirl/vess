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

// ------------------------------------------------------------------------
// Constructor - Initializes variables and identifies the slaves to which
// the master should connect
// ------------------------------------------------------------------------

vsClusterConfig::vsClusterConfig(const char *master, const int slaveCount, 
        const char **slaves)
{
    struct hostent  *ip;
    int             i;
    
    legitimate = 0;
    
    // Identify the master
    ip = gethostbyname(master);
    
    // If the master can't be found, there is a problem, so return
    if (!ip || !ip->h_addr_list[0])
    {
        printf("vsClusterConfig::vsClusterConfig: Can't locate master\n");
        slaveAddress = NULL;
        return;
    }
    
    // Store the master's IP address
    memcpy(masterAddress, ip->h_addr_list[0], 4*sizeof(char));
    
    // Prepare space for slave addresses
    numOfSlaves = slaveCount;
    slaveAddress = (unsigned char**) calloc(slaveCount, sizeof(char*));

    // Loop through each slave
    for (i = 0; i < slaveCount; i++)
    {
        
        // Get the slave's IP address
        ip = gethostbyname(slaves[i]);

        // If the slave can't be located, then there is a problem
        if (!ip || !ip->h_addr_list[0])
        {
            printf("vsClusterConfig::vsClusterConfig: Can't locate slave %s\n",
                    slaves[i]);
            slaveAddress[i] = NULL;
            return;
        }
        
        // Store the address of the slave
        slaveAddress[i] = (unsigned char*)calloc(4,sizeof(char));
        memcpy(slaveAddress[i],ip->h_addr_list[0],4);
    }

    // A cluster configuration is valid only if all addresses are valid
    legitimate = 1;
}

// ------------------------------------------------------------------------
// Destructor - Frees memory used to store addresses
// ------------------------------------------------------------------------
vsClusterConfig::~vsClusterConfig(void)
{
    int i;

    // If there are slave addresses
    if (slaveAddress)
    {
        // Loop through each slave, deleting it
        for (i = 0; i < numOfSlaves && slaveAddress[i]; i++)
        {
            free(slaveAddress[i]);
        }
        
        // Delete the array of addresses
        free(slaveAddress);
    }
}

// ------------------------------------------------------------------------
// Returns true if the object can be used
// ------------------------------------------------------------------------
bool vsClusterConfig::isValid(void)
{
    return legitimate;
}

// ------------------------------------------------------------------------
// Returns a pointer to the master's address
// ------------------------------------------------------------------------
const unsigned char* vsClusterConfig::getMaster(void)
{
    return masterAddress;
}

// ------------------------------------------------------------------------
// Returns a pointer to the specified slave
// ------------------------------------------------------------------------
const unsigned char* vsClusterConfig::getSlave(int slaveIndex)
{
    // Check to see that the index is within range
    if (slaveIndex >= 0 && slaveIndex < numOfSlaves)
    {
        return slaveAddress[slaveIndex];
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Returns the number of slaves
// ------------------------------------------------------------------------
int vsClusterConfig::numSlaves(void)
{
    return numOfSlaves;
}
