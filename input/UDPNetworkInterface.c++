
#include "UDPNetworkInterface.h++"


UDPNetworkInterface::UDPNetworkInterface(int blocking) : NetworkInterface()
{
   if ( (socket_value = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
      perror("socket");

   if (!blocking)
   {
      // Set to non blocking
      if (fcntl(socket_value, F_SETFL, FNDELAY) < 0)
         perror("fcntl");
   }
}


UDPNetworkInterface::~UDPNetworkInterface()
{
   close(socket_value);
}


int UDPNetworkInterface::readPacket(u_char *buffer, int maxSize)
{
   int   length;

   read_name_length = sizeof(read_name);
   length = recvfrom(socket_value, buffer, maxSize, 0,
                     (struct sockaddr *) &read_name, 
                     (socklen_t *) &read_name_length);

   if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("recvfrom");

   return (length);
}


int UDPNetworkInterface::readPacket(u_char *buffer, int maxSize,
                                    struct timeval *packetTime)
{
   int   length;

   read_name_length = sizeof(read_name);
   length = recvfrom(socket_value, buffer, maxSize, 0,
                     (struct sockaddr *) &read_name,
                     (socklen_t *) &read_name_length);

   if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("recvfrom");
   else
      gettimeofday(packetTime, NULL);

   return (length);
}


int UDPNetworkInterface::readPacket(u_char *buffer, int maxSize, char *origin)
{
   int    length;
   char   *addr;

   read_name_length = sizeof(read_name);
   length = recvfrom(socket_value, buffer, maxSize, 0,
                     (struct sockaddr *) &read_name,
                     (socklen_t *) &read_name_length);

   if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("recvfrom");

   addr = (char *) &read_name.sin_addr.s_addr;
   sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

   return (length);
}


int UDPNetworkInterface::readPacket(u_char *buffer, int maxSize, 
                                    struct timeval *packetTime, char *origin)
{
   int    length;
   char   *addr;

   read_name_length = sizeof(read_name);
   length = recvfrom(socket_value, buffer, maxSize, 0,
                     (struct sockaddr *) &read_name,
                     (socklen_t *) &read_name_length);

   if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("recvfrom");
   else
      gettimeofday(packetTime, NULL);

   addr = (char *) &read_name.sin_addr.s_addr;
   sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

   return (length);
}


int UDPNetworkInterface::writePacket(u_char *buffer, int length)
{
   int   error;

   error = sendto(socket_value, buffer, length, 0,
                  (struct sockaddr *) &write_name, write_name_length);

   if ( (error == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("sendto");

   return (error);
}

