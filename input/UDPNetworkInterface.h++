
#ifndef UDP_NETWORK_INTERFACE_H
#define UDP_NETWORK_INTERFACE_H


// INCLUDES
#include <unistd.h>
#include <fcntl.h>
#include "NetworkInterface.h++"


class UDPNetworkInterface : public NetworkInterface
{
   public:
      UDPNetworkInterface(int blocking);
      virtual   ~UDPNetworkInterface();
      int   readPacket(u_char *buffer);
      int   readPacket(u_char *buffer, int maxSize);
      int   readPacket(u_char *buffer, struct timeval *packetTime);
      int   readPacket(u_char *buffer, int maxSize, struct timeval *packetTime);      int   readPacket(u_char *buffer, char *origin);
      int   readPacket(u_char *buffer, int maxSize, char *origin);
      int   readPacket(u_char *buffer, struct timeval *packetTime,
                       char *origin);
      int   readPacket(u_char *buffer, int maxSize, struct timeval *packetTime,
                       char *origin);
      int   writePacket(u_char *buffer, int length);
};

#endif
