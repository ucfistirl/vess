
#include <string.h>
#include "vsRemoteInterface.h++"


#define VS_RI_MAX_BUFFER_SIZE   65536


// ------------------------------------------------------------------------
// Constructor - Initializes variables and opens up the TCP server for
// listening
// ------------------------------------------------------------------------
vsRemoteInterface::vsRemoteInterface()
{
    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(VS_RI_DEFAULT_CONTROL_PORT);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


// ------------------------------------------------------------------------
// Constructor - Initializes variables (including a new port to listen on
// rather than the default port) and opens up the TCP server for listening
// ------------------------------------------------------------------------
vsRemoteInterface::vsRemoteInterface(short port)
{
    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(port);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


// ------------------------------------------------------------------------
// Destructor - Closes up the TCP network interface and frees the memory
// used by the DTD file
// ------------------------------------------------------------------------
vsRemoteInterface::~vsRemoteInterface()
{
   // Close up the network interface (we check pointer just for safety)
   if (tcpInterface != NULL)
      delete tcpInterface;
}


// ------------------------------------------------------------------------
// The main update loop that reads the network and buffers it up to get
// a full XML document, which is then calls the buffer to process
// ------------------------------------------------------------------------
void vsRemoteInterface::update()
{
    int      tempClientID;
    int      lengthRead;
    u_long   i;
    u_char   buffer[VS_RI_MAX_XML_DOCUMENT_SIZE + VS_RI_MAX_BUFFER_SIZE];
    u_char   *response;

    // See if somebody wants to connect through the TCP interface
    tempClientID = tcpInterface->acceptConnection();
    if (tempClientID != -1)
    {
        // Somebody connected
        printf("Accepted connection.\n");

        // Set the new socket to be non-blocking (we don't want to get stuck
        // reading later)
        tcpInterface->disableBlockingOnClient(tempClientID);

        // Initialize the client's buffer and store the client ID
        tcpClientBuffers[numClients] = new vsRemoteInterfaceBuffer();
        tcpClientIDs[numClients] = tempClientID;
        numClients++;
    }

    // Bail out if we don't have a client yet
    if (numClients == 0)
        return;

    // Go through each connected client
    for (i=0; i < numClients; i++)
    {
        // Try to get something from the TCP stream
        lengthRead = tcpInterface->read(tcpClientIDs[i], buffer, 
                                        sizeof(buffer));

        // See if this client closed the connection or if it sent some bytes
        if (lengthRead == 0)
        {
            // Seems like the client disconnected

            // Get rid of the remote interface buffer for this client
            delete tcpClientBuffers[i];

            // Delete the client entry in our client lists
            memmove(&tcpClientBuffers[i], &tcpClientBuffers[i+1],
                    (numClients - i - 1) * sizeof(vsRemoteInterfaceBuffer *));
            memmove(&tcpClientIDs[i], &tcpClientIDs[i+1],
                    (numClients - i - 1) * sizeof(int));
            numClients--;
        }
        else if (lengthRead > 0)
        {
            // We received at least part of an XML document so process it
            response = tcpClientBuffers[i]->processBuffer(buffer, lengthRead);

            // Write the response if there is one
            if (strlen((char *) response) > 0)
            {
                // Send the output out the tcp interface we are processing
                tcpInterface->write(tcpClientIDs[i], response, 
                                    strlen((char *) response));
            }
        }
    }
}


// ------------------------------------------------------------------------
// Method to write a buffer back out to all remote interfaces 
// ------------------------------------------------------------------------
void vsRemoteInterface::send(u_char *buffer, u_long bufferLen)
{
    u_long   i;

    // Loop through the connected clients
    for (i=0; i < numClients; i++)
    {
        // Write out the buffer to the network
        tcpInterface->write(buffer, bufferLen);
    }
}

