
#ifndef VS_REMOTE_INTERFACE_BUFFER_HPP
#define VS_REMOTE_INTERFACE_BUFFER_HPP

#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "vsQuat.h++"
#include "vsSequencer.h++"

#define VS_RI_MAX_XML_DOCUMENT_SIZE    65536

class vsRemoteInterfaceBuffer
{
protected:
    u_char                   xmlBuffer[VS_RI_MAX_XML_DOCUMENT_SIZE];
    u_long                   xmlBufferSize;

    xmlDtdPtr                xmlDTD;
    xmlValidCtxt             xmlContext;

    void                     getPosition(xmlDocPtr doc, xmlNodePtr node,
                                         double *x, double *y, double *z);
    void                     getOrientation(xmlDocPtr doc, xmlNodePtr node,
                                            vsQuat *quat);
    void                     getSequenceTree(vsSequencer *currentSequencer);

    void                     processXMLDocument();
    void                     processPlaceComponent(xmlDocPtr doc, 
                                                   xmlNodePtr current);
    void                     processQuerySequence(xmlDocPtr doc, 
                                                  xmlNodePtr current);
    void                     processReleaseSync(xmlDocPtr doc, 
                                                xmlNodePtr current);
    void                     processSetKinematics(xmlDocPtr doc, 
                                                  xmlNodePtr current);
    void                     processStats(xmlDocPtr doc, xmlNodePtr current);
    void                     processTerminateCluster(xmlDocPtr doc, 
                                                     xmlNodePtr current);

public:
    vsRemoteInterfaceBuffer();
    vsRemoteInterfaceBuffer(char *dtdFilename);
    virtual ~vsRemoteInterfaceBuffer();

    void    processBuffer(u_char *buffer, int lengthRead);
};

#endif

