
#include "UDPUnicastNetworkInterface.h++"


UDPUnicastNetworkInterface::UDPUnicastNetworkInterface(char *address, short port, int blocking) : UDPNetworkInterface(blocking)
{
   struct hostent   *host;
   int              on;
   int              max;

   // Initialize the read name field
   read_name.sin_family = AF_INET;
   read_name.sin_addr.s_addr = htonl(INADDR_ANY);
   read_name.sin_port = htons(port);

   // Get information about remote host
   host = gethostbyname(address);

   // Initialize the write name field
   write_name.sin_family = AF_INET;
   bcopy(host->h_addr_list[0], &write_name.sin_addr.s_addr, host->h_length);
   write_name.sin_port = htons(port);

   // Set the options we need on the socket
   on = 1;
   max = MAX_PACKET_SIZE;
   if (setsockopt(socket_value, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
      perror("setsockopt broadcast");
   if (setsockopt(socket_value, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
      perror("setsockopt reuseport");
   if (setsockopt(socket_value, SOL_SOCKET, SO_SNDBUF, &max, sizeof(max)) < 0)
      perror("setsockopt sndbuf");
   if (setsockopt(socket_value, SOL_SOCKET, SO_RCVBUF, &max, sizeof(max)) < 0)
      perror("setsockopt rcvbuf");

   // Bind to the port
   if ( bind(socket_value, (struct sockaddr *) &read_name, 
             sizeof(read_name)) < 0 )
      perror("bind");
}


UDPUnicastNetworkInterface::~UDPUnicastNetworkInterface()
{
}

