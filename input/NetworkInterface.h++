
#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H


// INCLUDES
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#ifdef __linux__
#include <string.h>
#else
#include <bstring.h>
#endif
#include <errno.h>


// CONSTANTS
#define MAX_PACKET_SIZE   65000


class NetworkInterface
{
   protected:
      int                  socket_value;
      struct sockaddr_in   read_name;
      struct sockaddr_in   write_name;
      int                  read_name_length;
      int                  write_name_length;

   public:
      NetworkInterface();
      virtual   ~NetworkInterface();
      virtual int    readPacket(u_char *buffer, int size) = 0;
      virtual int    readPacket(u_char *buffer, int size,
                                struct timeval *packetTime) = 0;
      virtual int    readPacket(u_char *buffer, int size, char *origin) = 0;
      virtual int    readPacket(u_char *buffer, int size, 
                                struct timeval *packetTime, char *origin) = 0;
      virtual int    writePacket(u_char *buffer, int length) = 0;
};

#endif

