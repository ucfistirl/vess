
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
//    VESS Module:  vsTCPNetworkInterface.c++
//
//    Description:  Class supporting TCP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include "vsTCPNetworkInterface.h++"

// ------------------------------------------------------------------------
// Constructor, opens a socket to the given address on the given port
// ------------------------------------------------------------------------
vsTCPNetworkInterface::vsTCPNetworkInterface(char *address, u_short port)
{
    char              hostname[MAXGETHOSTSTRUCT];
    struct hostent    *host;

    // Open the socket
    if ( (socketValue = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        printf("Unable to open socket for communication.\n");

    // Get information about this host and initialize the read name field
    gethostname(hostname, sizeof(hostname));
    host = gethostbyname(hostname);
    readName.sin_family = AF_INET;
    memcpy(&readName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    readName.sin_port = htons(port);

    // Get information about remote host and initialize the write name field
    host = gethostbyname(address);
    writeName.sin_family = AF_INET;
    memcpy(&writeName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    writeName.sin_port = htons(port);

    // Initialize remaining instance variables
    numClientSockets = 0;
}

// ------------------------------------------------------------------------
// Constructor, opens a socket to the local address on the given port
// ------------------------------------------------------------------------
vsTCPNetworkInterface::vsTCPNetworkInterface(u_short port)
{
    char              hostname[MAXGETHOSTSTRUCT];
    struct hostent    *host;

    // Open the socket
    if ( (socketValue = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        printf("Unable to open socket for communication.\n");

    // Get information about this host and initialize the read name field
    gethostname(hostname, sizeof(hostname));
    host = gethostbyname(hostname);
    readName.sin_family = AF_INET;
    memcpy(&readName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    readName.sin_port = htons(port);

    // Also copy the information about this host into the write name field
    writeName.sin_family = AF_INET;
    memcpy(&writeName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    writeName.sin_port = htons(port);

    // Initialize remaining instance variables
    numClientSockets = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTCPNetworkInterface::~vsTCPNetworkInterface()
{
    // Close the socket
    closesocket(socketValue);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTCPNetworkInterface::getClassName()
{
    return "vsTCPNetworkInterface";
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTCPNetworkInterface::getClassName()
{
    return "vsTCPNetworkInterface";
}

// ------------------------------------------------------------------------
// Binds to the socket and indicates the willingness to take connections
// ------------------------------------------------------------------------
void vsTCPNetworkInterface::allowConnections(int backlog)
{
    // Bind to the port
    if (bind(socketValue, (struct sockaddr *) &readName, sizeof(readName)) < 0)
    {
        printf("Unable to bind to the port.\n");
    }

    // Notify our willingness to accept connections and give a backlog limit
    listen(socketValue, backlog);
}

// ------------------------------------------------------------------------
// Tries to accept a client connection (stores the client socket if there
// is indeed a connection)
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::acceptConnection()
{
    fd_set                readFDs;
    struct timeval        tv;
    SOCKET                newSocket;
    struct sockaddr_in    connectingName;
    int                   connectingNameLength;

    // Set up the file descriptor set to allow reading from our main socket
    FD_ZERO(&readFDs);
    FD_SET(socketValue, &readFDs);

    // Set up a time for 1 microsecond
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    // Bail out if there's nobody waiting (select returns -1 on error and
    // 0 if nobody is there)
    if (select((int)socketValue+1, &readFDs, NULL, NULL, &tv) < 1)
        return -1;

    // Try to accept a connection
    connectingNameLength = sizeof(connectingName);
    newSocket = accept(socketValue, (struct sockaddr *) &connectingName, 
                       &connectingNameLength);

    // If we had an error and it wasn't that we would block on a non-blocking
    // socket (a blocking socket shouldn't generate an EWOULDBLOCK error), then
    // notify the user; otherwise, store the socket and return an ID to the user
    if ( (newSocket == -1) && (errno != WSAEWOULDBLOCK) )
    {
        printf("Could not accept a connection.\n");
        return -1;
    }
    else
    {
        // Store the client socket
        clientSockets[numClientSockets] = newSocket;

        // Store the client name (and length)
        memcpy(&clientNames[numClientSockets], &connectingName,
               sizeof(connectingName));
        clientNameLengths[numClientSockets] = connectingNameLength;

        // Increment the count and return the index for this client
        numClientSockets++;
        return numClientSockets - 1;
    }
}

// ------------------------------------------------------------------------
// Enables blocking for reads and writes on the client connection socket
// ------------------------------------------------------------------------
void vsTCPNetworkInterface::enableBlockingOnClient(int clientID)
{
    // Disable nonblocking on the socket (Windows bases everything on 
    // *nonblocking*)
    nonblockingMode = 0;
    if (ioctlsocket(clientSockets[clientID], FIONBIO, 
        &nonblockingMode) == SOCKET_ERROR)
    {
        // If there was error, notify the user
        printf("Unable to disable blocking on socket.\n");
    }
}

// ------------------------------------------------------------------------
// Disables blocking for reads and writes on the client connection socket
// ------------------------------------------------------------------------
void vsTCPNetworkInterface::disableBlockingOnClient(int clientID)
{
    // Enable nonblocking on the socket
    nonblockingMode = 1;
    if (ioctlsocket(clientSockets[clientID], FIONBIO, 
        &nonblockingMode) == SOCKET_ERROR)
    {
        // If there was error, notify the user
        printf("Unable to disable blocking on socket.\n");
    }
}

// ------------------------------------------------------------------------
// If acting as a client, tries to connect to the server 
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::makeConnection()
{
    int                   keepTrying;
    struct sockaddr_in    connectingName;

    keepTrying = 1;
    while (keepTrying == 1)
    {
        // Try to connect
        connectingName = writeName;
        if (connect(socketValue, (struct sockaddr *) &connectingName,
            sizeof(connectingName)) != -1)
        {
            // We connected so signal the loop to end
            keepTrying = 0;
        }
        else
        {
            // We didn't connect so close the socket
            closesocket(socketValue);
            socketValue = -1;

            // If we are not in blocking mode, tell the loop to stop (we give 
            // up); Otherwise, tell the user the info that we failed this time
            // and re-open the socket
            if (nonblockingMode == 0)
                keepTrying = 0;
            else
            {
                printf("Failed to connect to server.  Trying again.\n");

                // Re-open the socket
                if ( (socketValue = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
                   printf("Unable to open socket for communication.\n");

                // Set the new socket to nonblocking 
                if (ioctlsocket(socketValue, FIONBIO, &nonblockingMode) == SOCKET_ERROR)
                   printf("Unable to disable blocking on socket.\n");
            }
        }
    }

    // Tell the user whether or not we succeeded to connect
    if (socketValue == -1)
        return -1;
    else
    {
        // Store the socket as a client
        clientSockets[numClientSockets] = socketValue;

        // Store the client name (and length)
        memcpy(&clientNames[numClientSockets], &connectingName,
               sizeof(connectingName));
        clientNameLengths[numClientSockets] = sizeof(connectingName);

        // Increment the count and return that we succeeded
        numClientSockets++;
        return 0;
    }
}

// ------------------------------------------------------------------------
// Reads up to len bytes from the socket into the buffer and returns
// the actual number of bytes read (this is for client mode or if
// you only have a single client when a server)
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::read(u_char *buffer, u_long len)
{
    struct sockaddr_in    fromAddress;
    int                   fromAddressLength;
    int                   packetLength;

    // Make sure we have a client, error if not
    if (numClientSockets > 0)
    {
        // Get a packet
        fromAddressLength = sizeof(fromAddress);
        packetLength = recvfrom(clientSockets[0], (char *) buffer, len, 0, 
                                (struct sockaddr *) &fromAddress, 
                                &fromAddressLength);

        // Check if the client has closed the connection
        if (packetLength == 0)
        {
            // Close the socket
            closesocket(clientSockets[0]);

            // Remove the socket from our list
            numClientSockets--;

            // Return that the connection closed
            return 0;
        }
        else
        {
            // Tell user how many bytes we read (-1 means an error)
            return packetLength;
        }
    }
    else
       return -1;
}

// ------------------------------------------------------------------------
// Reads up to len bytes from the clientID's socket into the buffer and 
// returns the actual number of bytes read
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::read(int clientID, u_char *buffer, u_long len)
{
    struct sockaddr_in   fromAddress;
    int                  fromAddressLength;
    int                  packetLength;

    // Get a packet
    fromAddressLength = sizeof(fromAddress);
    packetLength = recvfrom(clientSockets[clientID], (char *) buffer, len, 0,
                            (struct sockaddr *) &fromAddress, 
                            &fromAddressLength);

    // Check if the client has closed the connection
    if (packetLength == 0)
    {
        // Close the socket
        closesocket(clientSockets[clientID]);

        // Remove the socket from our list
        if (VS_MAX_TCP_CLIENTS > 1)
        {
            // Slide the list down if it's more one socket in size
            memmove(&clientSockets[clientID], &clientSockets[clientID+1],
                (numClientSockets - clientID) * sizeof(int));
            memmove(&clientNames[clientID], &clientNames[clientID+1],
                (numClientSockets - clientID) * sizeof(struct sockaddr_in));
            memmove(&clientNameLengths[clientID], 
                &clientNameLengths[clientID+1],
                (numClientSockets - clientID) * sizeof(int));
        }
        numClientSockets--;

        // Return that the connection closed
        return 0;
    }
    else
    {
        // Tell user how many bytes we read (-1 means an error)
        return packetLength;
    }
}

// ------------------------------------------------------------------------
// Writes a packet of the specified length containing the data in buffer
// to the socket (again, if acting as a client or if you have only one
// client when acting as a server)
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::write(u_char *buffer, u_long len)
{
    fd_set            writeFDs;
    struct timeval    tv;
    int               status;
    int               lengthWritten;

    // Set up the file descriptor set to allow reading from our main socket
    FD_ZERO(&writeFDs);
    FD_SET(clientSockets[0], &writeFDs);

    // Set up a time for 1 microsecond
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    // Wait for the socket to have space to write some bytes in (this will
    // return immediately if there is space).  This lets us take a split
    // second to see if there will be space rather than returning back to
    // the user immediately and having them, sleep and loop, etc.
    status = select((int)clientSockets[0] + 1, NULL, &writeFDs, NULL, &tv);
    if (status < 1)
        return status;

    // Make sure we have a client, error if not
    if (numClientSockets > 0)
    {
        // Write the packet
        lengthWritten = sendto(clientSockets[0], (char *)buffer, len, 0, 
                               (struct sockaddr *) &clientNames[0], 
                               clientNameLengths[0]);

        // Tell user how many bytes we wrote (-1 if error)
        return lengthWritten;
    }
    else
        return -1;
}

// ------------------------------------------------------------------------
// Writes a packet of the specified length containing the data in buffer
// to the clientID's socket
// ------------------------------------------------------------------------
int vsTCPNetworkInterface::write(int clientID, u_char *buffer, u_long len)
{
    fd_set           writeFDs;
    struct timeval   tv;
    int              status;
    int              lengthWritten;

    // Set up the file descriptor set to allow reading from our main socket
    FD_ZERO(&writeFDs);
    FD_SET(socketValue, &writeFDs);

    // Set up a time for 1 microsecond
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    // Wait for the socket to have space to write some bytes in (this will
    // return immediately if there is space).  This lets us take a split
    // second to see if there will be space rather than returning back to
    // the user immediately and having them, sleep and loop, etc.
    status = select((int)clientSockets[clientID] + 1, NULL, &writeFDs, NULL, &tv);
    if (status < 1)
        return status;

    // Write the packet
    lengthWritten = sendto(clientSockets[clientID], (char *)buffer, len, 0,  
                           (struct sockaddr *) &clientNames[clientID],
                           clientNameLengths[clientID]);

    // Tell user how many bytes we wrote (-1 if error)
    return lengthWritten;
}

