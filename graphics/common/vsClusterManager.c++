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
//    VESS Module:  vsClusterManager.c++
//
//    Description:  Connects to machines in a cluster.
//                  
//
//    Author(s):    Ben Douglass
//
//------------------------------------------------------------------------

#include "vsClusterManager.h++"
#include "vsTCPNetworkInterface.h++"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

vsClusterManager *vsClusterManager::clusterManagerObject = NULL;

// ------------------------------------------------------------------------
// Constructor - Initializes variables and identifies the slaves to which
// the master should connect
// ------------------------------------------------------------------------
vsClusterManager::vsClusterManager(const int slaveCount, char **newSlaves)
{
    int i;
    
    legitimate = 0;
    
    // There can exist only one cluster manager
    if (clusterManagerObject)
    {
        printf("vsClusterManager::vsClusterManager: Only one vsClusterObject "
               "can be in existence at a time\n");
        return;
    }
    
    // When the program begins, there haven't yet been any calls generated
    // internally by VESS   
    stackDepth = 0;
 
    // Prepare space for slave addresses
    numOfSlaves = slaveCount;
    
    // Make space for interfaces with slaves
    slaves = (vsTCPNetworkInterface**)calloc(numOfSlaves, 
            sizeof(vsTCPNetworkInterface *));
        
    // Connect to each slave
    for (i = 0; i < numOfSlaves; i++)
    {   
        // Now try to connect
        slaves[i] = new vsTCPNetworkInterface(newSlaves[i], 
                VS_RI_DEFAULT_CONTROL_PORT);
        while (slaves[i]->makeConnection() < 0);
        slaves[i]->disableBlocking();
    }

    
    // A cluster configuration is valid only if all addresses are valid
    legitimate = 1;
    clusterManagerObject = this;
}

// ------------------------------------------------------------------------
// Constructor - Initializes variables and identifies the slaves to which
// the master should connect. The port which we should use to connect is 
// specified
// ------------------------------------------------------------------------
vsClusterManager::vsClusterManager(const int slaveCount, char **newSlaves,
        int port)
{
    int i;
    
    legitimate = 0;
    
    // There can exist only one cluster manager
    if (clusterManagerObject)
    {
        printf("vsClusterManager::vsClusterManager: Only one vsClusterObject "
               "can be in existence at a time\n");
        return;
    }
    
    // When the program begins, there haven't yet been any calls generated
    // internally by VESS   
    stackDepth = 0;
 
    // Prepare space for slave addresses
    numOfSlaves = slaveCount;
    
    // Make space for interfaces with slaves
    slaves = (vsTCPNetworkInterface**)calloc(numOfSlaves, 
            sizeof(vsTCPNetworkInterface *));
        
    // Connect to each slave
    for (i = 0; i < numOfSlaves; i++)
    {   
        // Now try to connect
        slaves[i] = new vsTCPNetworkInterface(newSlaves[i], port);
        while (slaves[i]->makeConnection() < 0);
        slaves[i]->disableBlocking();
    }

    
    // A cluster configuration is valid only if all addresses are valid
    legitimate = 1;
    clusterManagerObject = this;
}


// ------------------------------------------------------------------------
// Destructor - Frees memory used to store addresses
// ------------------------------------------------------------------------
vsClusterManager::~vsClusterManager(void)
{
    int i;

    // If there are slaves
    if (slaves)
    {
        // Loop through each slave, deleting it
        for (i = 0; i < numOfSlaves && slaves[i]; i++)
        {
            free(slaves[i]);
        }
        
        // Delete the array of addresses
        free(slaves);
    }

    // If the true cluster manager is being deleted, set it to null
    if (clusterManagerObject == this)
        clusterManagerObject = NULL;
}

// ------------------------------------------------------------------------
// Returns true if the object can be used
// ------------------------------------------------------------------------
bool vsClusterManager::isValid(void)
{
    return legitimate;
}

// ------------------------------------------------------------------------
// Returns a pointer to the specified slave
// ------------------------------------------------------------------------
const vsTCPNetworkInterface *vsClusterManager::getSlave(int slaveIndex)
{
    // Check to see that the index is within range
    if (slaveIndex >= 0 && slaveIndex < numOfSlaves)
    {
        return slaves[slaveIndex];
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Returns the number of slaves
// ------------------------------------------------------------------------
int vsClusterManager::numSlaves(void)
{
    return numOfSlaves;
}

// ------------------------------------------------------------------------
// Sends the specified message to all of the slaves
// ------------------------------------------------------------------------
void vsClusterManager::transmit(char *commStr)
{
    int i;
    int messageLength;

    // Record the length of the message (remember to count the null 
    // terminator!)
    messageLength = strlen(commStr) + 1;
    
    // Loop through each slave
    for (i = 0; i < numOfSlaves; i++)
    {
        // Send the message
        slaves[i]->write((u_char *) commStr, messageLength);
    }
}

// ------------------------------------------------------------------------
// Blocks until all slaves report that they are ready to swap
// ------------------------------------------------------------------------
void vsClusterManager::sync()
{
    int i;
    int numSlavesReportedIn;
    char commStr[1024];
    
    // Block until all clients acknowledge
    numSlavesReportedIn = 0;
    while (numSlavesReportedIn < numOfSlaves)
    {
        // Collect messages from each slave
        for (i = 0; i < numOfSlaves; i++)
        {
            slaves[i]->read((u_char *) commStr, 1024);
            
            // If the syncing signal has been received
            if (!strcmp(commStr, "<?xml version=\"1.0\"?>\n"
                    "<vessxml version=\"1.0\">\n"
                    "<readytosync>\n"
                    "</readytosync>\n"
                    "</vessxml>"))
            {
                // Mark down that we've gotten a message from a client
                numSlavesReportedIn++;

                // Make sure that we don't get the next message confused
                // with this one
                commStr[0]='\0';
            }
        }
    }
    
    // Send relase signal
    transmit("<?xml version=\"1.0\"?>\n"
             "<vessxml version=\"1.0\">\n"
             "<releasesync>\n"
             "</releasesync>\n"
             "</vessxml>");
}

// ------------------------------------------------------------------------
// Internal Function
// Returns the current number of internally generated VESS calls
// ------------------------------------------------------------------------
int vsClusterManager::getStackDepth()
{
    return stackDepth;
}

// ------------------------------------------------------------------------
// Internal Function
// Should be called upon entrance to a VESS function that calls other VESS
// functions so that those calls will not be transmitted across the network
// ------------------------------------------------------------------------
void vsClusterManager::stackIncrement()
{
    stackDepth++;
}

// ------------------------------------------------------------------------
// Internal Function
// Should be called upon exiting a function that called stackIncrement() so
// that function calls can again be transmitted
// ------------------------------------------------------------------------
void vsClusterManager::stackDecrement()
{
    // The stack depth should NEVER fall below zero
    if(!stackDepth)
        printf("vsClusterManager::stackDecrement(): Decrement without "
               "increment!\n");
    stackDepth--;
}
