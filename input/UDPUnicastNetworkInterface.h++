
#ifndef UDP_UNICAST_NETWORK_INTERFACE_H
#define UDP_UNICAST_NETWORK_INTERFACE_H


// INCLUDES
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "UDPNetworkInterface.h++"


class UDPUnicastNetworkInterface : public UDPNetworkInterface
{
   public:
      UDPUnicastNetworkInterface(char *address, short port, int blocking);
      ~UDPUnicastNetworkInterface();
};

#endif

