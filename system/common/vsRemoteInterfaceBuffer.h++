
#ifndef VS_REMOTE_INTERFACE_BUFFER_HPP
#define VS_REMOTE_INTERFACE_BUFFER_HPP

#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "atQuat.h++"
#include "vsSequencer.h++"

#define VS_RI_MAX_XML_DOCUMENT_SIZE    65536

class VESS_SYM vsRemoteInterfaceBuffer
{
protected:
    u_char                   xmlBuffer[VS_RI_MAX_XML_DOCUMENT_SIZE];
    u_long                   xmlBufferSize;

    xmlDtdPtr                xmlDTD;
    xmlValidCtxt             xmlContext;

    u_char                   xmlResponses[VS_RI_MAX_XML_DOCUMENT_SIZE];

    float                    xmlToFloat(xmlChar *tmpStr);
    int                      xmlToInt(xmlChar *tmpStr);

    void                     getPosition(xmlDocPtr doc, xmlNodePtr node,
                                         double *x, double *y, double *z);
    void                     getOrientation(xmlDocPtr doc, xmlNodePtr node,
                                            atQuat *quat);
    void                     getSequenceTree(vsSequencer *currentSequencer,
                                             char *sequenceTreeBuffer);

    void                     processXMLDocument();
    void                     processPlaceComponent(xmlDocPtr doc, 
                                                   xmlNodePtr current);
    void                     processQuerySequence(xmlDocPtr doc, 
                                                  xmlNodePtr current);
    void                     processSetKinematics(xmlDocPtr doc, 
                                                  xmlNodePtr current);
    void                     processSetSequence(xmlDocPtr doc, 
                                                xmlNodePtr current,
                                                vsSequencer *currentSequencer);
    void                     processStats(xmlDocPtr doc, xmlNodePtr current);

public:
    vsRemoteInterfaceBuffer();
    vsRemoteInterfaceBuffer(char *dtdFilename);
    virtual ~vsRemoteInterfaceBuffer();

    u_char *    processBuffer(u_char *buffer, int lengthRead);
};

#endif

