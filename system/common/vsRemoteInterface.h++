
#ifndef VS_REMOTE_INTERFACE_HPP
#define VS_REMOTE_INTERFACE_HPP

#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsTCPNetworkInterface.h++"
#include "vsQuat.h++"

#define VS_RI_DEFAULT_CONTROL_PORT     32816
#define VS_RI_MAX_XML_DOCUMENT_SIZE    65536

#define VS_MAX_CONNECTIONS   10

class vsRemoteInterface
{
protected:
    u_char                   xmlBuffer[VS_RI_MAX_XML_DOCUMENT_SIZE];
    u_long                   xmlBufferSize;

    xmlDtdPtr                xmlDTD;
    xmlValidCtxt             xmlContext;

    vsTCPNetworkInterface    *tcpInterface;

    u_long                   numClients;
    int                      tcpClientIDs[VS_MAX_CONNECTIONS];

    void                     processXMLDocument();
    void                     processStats(xmlDocPtr doc, xmlNodePtr current);

public:
    vsRemoteInterface();
    vsRemoteInterface(char *dtdFilename);
    vsRemoteInterface(short port);
    vsRemoteInterface(char *dtdFilename, short port);
    virtual ~vsRemoteInterface();

    void   update();
    void   send(u_char *buffer, u_long bufferLen);
};

#endif

