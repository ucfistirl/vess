
#include "NetworkInterface.h++"


NetworkInterface::NetworkInterface()
{
   // Clear the name fields
   read_name_length = sizeof(read_name);
   write_name_length = sizeof(write_name);
   bzero((char *) &read_name, read_name_length);
   bzero((char *) &write_name, write_name_length);
}


NetworkInterface::~NetworkInterface()
{
}

