
#ifndef VS_REMOTE_INTERFACE_HPP
#define VS_REMOTE_INTERFACE_HPP

#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsTCPNetworkInterface.h++"
#include "vsUDPNetworkInterface.h++"
#include "vsQuat.h++"

#define VESS_RI_DEFAULT_CONTROL_PORT     3280
#define VESS_RI_MAX_XML_DOCUMENT_SIZE    65536

class vsRemoteInterface
{
protected:
    u_char                   xmlBuffer[VESS_RI_MAX_XML_DOCUMENT_SIZE];
    u_long                   xmlBufferSize;

    xmlDtdPtr                xmlDTD;
    xmlValidCtxt             xmlContext;

    vsTCPNetworkInterface    *tcpInterface;
    vsUDPNetworkInterface    *udpInterface;

    int                      tcpClientID;

    void                     processBuffer();
    void                     processStats(xmlDocPtr doc, xmlNodePtr current);

public:
    vsRemoteInterface();
    vsRemoteInterface(char *dtdFilename);
    vsRemoteInterface(short port);
    vsRemoteInterface(char *dtdFilename, short port);
    virtual ~vsRemoteInterface();

    void   update();
};

#endif

