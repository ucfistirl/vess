
#include <string.h>
#include "vsRemoteInterface.h++"
#include "vsScreen.h++"
#include "vsWindow.h++"
#include "vsPane.h++"

#define VS_RI_MAX_BUFFER_SIZE   65536

vsRemoteInterface::vsRemoteInterface()
{
    // Initialize the xml DTD (we aren't using one in this case and also
    // don't need a context then)
    xmlDTD = NULL;

    // Initialize the stored (buffered) xml buffer
    xmlBuffer[0] = '\0';
    xmlBufferSize = 0;

    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(VS_RI_DEFAULT_CONTROL_PORT);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


vsRemoteInterface::vsRemoteInterface(char *dtdFilename)
{
    // Open and initialize the DTD file (used for validating XML)
    xmlDTD = xmlParseDTD(NULL, (const xmlChar *) dtdFilename);

    // Initialize the stored (buffered) xml buffer
    xmlBuffer[0] = '\0';
    xmlBufferSize = 0;

    // Initialize the XML context information (used during XML validation
    // to print warnings and errors)
    xmlContext.userData = stderr;
    xmlContext.error = (xmlValidityErrorFunc ) fprintf;
    xmlContext.warning = (xmlValidityWarningFunc ) fprintf;

    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(VS_RI_DEFAULT_CONTROL_PORT);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


vsRemoteInterface::vsRemoteInterface(short port)
{
    // Initialize the xml DTD (we aren't using one in this case and also
    // don't need a context then)
    xmlDTD = NULL;

    // Initialize the stored (buffered) xml buffer
    xmlBuffer[0] = '\0';
    xmlBufferSize = 0;

    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(port);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


vsRemoteInterface::vsRemoteInterface(char *dtdFilename, short port)
{
    // Open and initialize the DTD file (used for validating XML)
    xmlDTD = xmlParseDTD(NULL, (const xmlChar *) dtdFilename);

    // Initialize the stored (buffered) xml buffer
    xmlBuffer[0] = '\0';
    xmlBufferSize = 0;

    // Initialize the XML context information (used during XML validation
    // to print warnings and errors)
    xmlContext.userData = stderr;
    xmlContext.error = (xmlValidityErrorFunc ) fprintf;
    xmlContext.warning = (xmlValidityWarningFunc ) fprintf;

    // Open the TCP connection and listen for clients
    tcpInterface = new vsTCPNetworkInterface(port);
    tcpInterface->allowConnections(1);
    tcpInterface->disableBlocking();

    // Initialize the client ID storage
    numClients = 0;
}


vsRemoteInterface::~vsRemoteInterface()
{
   // Close up the network interface (we check pointer just for safety)
   if (tcpInterface != NULL)
      delete tcpInterface;

   // Free up the DTD if we created it
   if (xmlDTD != NULL)
      xmlFreeDtd(xmlDTD);
}


void vsRemoteInterface::processXMLDocument()
{
    xmlDocPtr    doc;
    xmlNodePtr   current;
    xmlChar      *version;

    // Open the memory buffer as an XML document
    doc = xmlParseMemory((const char *) xmlBuffer, xmlBufferSize);

    // Check to make sure the XML library understood the buffer
    if (doc == NULL)
    {
        // We didn't recognize the XML at all so warn the user and bail out
        fprintf(stderr, "VESS XML document not parsed successfully.\n");
        xmlFreeDoc(doc);
        return;
    }

    // Check to see if we're checking the XML against the DTD
    if (xmlDTD != NULL)
    {
        // We are so go ahead and validate the XML against the DTD
        if (xmlValidateDtd(&xmlContext, doc, xmlDTD) == 0)
        {
            // The document failed the DTD check so warn the user and bail out
            fprintf(stderr, "VESS XML document not valid.\n");
            xmlFreeDoc(doc);
            return;
        }
    }

    // Ok, now let's get the XML root element (all non-empty XML documents
    // have a root)
    current = xmlDocGetRootElement(doc);

    // Check to see if we got something in the root (i.e. it's not an 
    // empty document)
    if (current == NULL)
    {
        fprintf(stderr, "VESS XML document is empty.\n");
        xmlFreeDoc(doc);
        return;
    }

    // Now, check to make sure the root element is the "vessxml" header
    // (all VESS XML documents start with "<vessxml>" and end with
    // "</vessxml>"
    if (xmlStrcmp(current->name, (const xmlChar *) "vessxml"))
    {
        fprintf(stderr, "VESS XML document is of wrong type.\n");
        xmlFreeDoc(doc);
        return;
    }

    // Get the version of VESS XML we're using from the "vessxml" header
    // (later on, we'll assume that NULL means 1.0)
    version = xmlGetProp(current, (const xmlChar *) "version");

    // Ok, all checks done so let's process!  Go through each child of the
    // root element and process it (each child will be a "command" of VESS
    // XML within the "vessxml" root element)
    current = current->xmlChildrenNode;
    while (current != NULL)
    {
        // Look at the child's name and process accordingly (the name will
        // match what we think of as VESS XML commands -- "stats" for
        // example)
        if (xmlStrcmp(current->name, (const xmlChar *) "stats") == 0)
        {
            processStats(doc, current);
        }

        // Get the next child of the root element
        current = current->next;
    }

    // We're done so free the document (and other stuff we're required to
    // free ourselves) up
    xmlFree(version);
    xmlFreeDoc(doc);
}


void vsRemoteInterface::processStats(xmlDocPtr doc, xmlNodePtr current)
{
    int       screenIndex;
    int       windowIndex;
    int       paneIndex;
    xmlChar   *displayMode;

    // Look for the required attributes
    screenIndex = atoi((const char *) xmlGetProp(current, 
                                                 (const xmlChar *) "screen"));
    windowIndex = atoi((const char *) xmlGetProp(current, 
                                                 (const xmlChar *) "window"));
    paneIndex = atoi((const char *) xmlGetProp(current, 
                                               (const xmlChar *) "pane"));
    displayMode = xmlGetProp(current, (const xmlChar *) "display");

    // Set the stats mode accordingly
    if (xmlStrcmp(displayMode, (const xmlChar *) "on") == 0)
    {
        // Make sure the stats are on for the specified pane
        vsScreen::getScreen(screenIndex)->getChildWindow(windowIndex)->
            getChildPane(paneIndex)->enableStats();
    }
    else
    {
        // Make sure the stats are off for the specified pane
        vsScreen::getScreen(screenIndex)->getChildWindow(windowIndex)->
            getChildPane(paneIndex)->disableStats();
    }
}


void vsRemoteInterface::update()
{
    int      tempClientID;
    u_long   i;
    int      lengthRead;
    u_char   buffer[VS_RI_MAX_XML_DOCUMENT_SIZE + VS_RI_MAX_BUFFER_SIZE];
    u_char   *nextNull;
    u_char   *endTag;
    u_long   partialChunkSize;
    u_long   numWhiteSpace;

    // See if somebody wants to connect through the TCP interface
    tempClientID = tcpInterface->acceptConnection();
    if (tempClientID != -1)
    {
        // Somebody connected
        printf("Accepted connection.\n");

        // Set the new socket to be non-blocking (we don't want to get stuck
        // reading later)
        tcpInterface->disableBlockingOnClient(tempClientID);

        // Store the client ID
        tcpClientIDs[numClients] = tempClientID;
        numClients++;
    }

    // Bail out if we don't have a client yet
    if (numClients == 0)
        return;

    // Try to get something from the TCP stream
    lengthRead = tcpInterface->read(tcpClientIDs[0], buffer, sizeof(buffer));

    // Determine if we received at least part of an XML document 
    if (lengthRead > 0)
    {
        // Go through and eradicate NULL characters (the XML library will
        // consider it the end of the document)
        nextNull = (u_char *) memchr(buffer, '\0', lengthRead);
        while (nextNull != NULL)
        {
            // Slide the buffer up one to get rid of this NULL
            memmove(nextNull, nextNull+1, lengthRead - (nextNull - buffer));
            lengthRead--;

            // Find the next NULL
            nextNull = (u_char *) memchr(buffer, '\0', lengthRead);
        }

        // Stick any partial buffer that was saved last go around in front
        // of the new buffer (this is necessary to cover the case when the
        // end tag "</vessxml>" gets split between packets
        memmove(&buffer[xmlBufferSize], &buffer[0], lengthRead);
        memcpy(&buffer[0], &xmlBuffer[0], xmlBufferSize);
        lengthRead += xmlBufferSize;
        xmlBufferSize = 0;

        // Make a string out of the buffer
        buffer[lengthRead] = '\0';

        // Check if we see the ending "</vessxml>"
        endTag = (u_char *) strstr((const char *) buffer, "</vessxml>");
        if (endTag != NULL)
        {
            // We need to loop in case there are multiple documents within 
            // the buffer
            while (endTag != NULL)
            {
                // Copy through the ending "</vessxml>" to the internal
                // collected xml buffer
                partialChunkSize = (endTag - &buffer[0]) + strlen("</vessxml>");
                memcpy(&xmlBuffer[xmlBufferSize], buffer, partialChunkSize);
                xmlBufferSize += partialChunkSize;

                // Delete the part we just used (+1 for the NULL we tacked on)
                memmove(&buffer[0], &buffer[partialChunkSize], 
                        lengthRead - partialChunkSize + 1);
                lengthRead -= partialChunkSize;

                // Eliminate any leading white space lines left in the buffer
                numWhiteSpace = strspn((const char *) buffer, " \r\n\t");
                if ( (numWhiteSpace > 0) && (numWhiteSpace <= lengthRead) )
                {
                    // Move up the remaining part of the buffer (+1 for the NULL
                    // we tacked on)
                    memmove(&buffer[0], &buffer[numWhiteSpace],
                            (lengthRead - numWhiteSpace + 1) * sizeof(u_char));
                    lengthRead -= numWhiteSpace;
                }

                // Process the document
                processXMLDocument();

                // Clear the internal xml buffer
                xmlBufferSize = 0;

                // See if there is another "</vessxml>" (in case there are two
                // or more within the buffer)
                endTag = (u_char *) strstr((const char *) buffer, "</vessxml>");
            }

            // Store remaining chunk (if any) in internal collected xml buffer
            if (lengthRead > 0)
            {
                // Copy remaining chunk into xml buffer and initialize
                // size to size of that remaining chunk
                memcpy(&xmlBuffer[0], &buffer[0], lengthRead);
                xmlBufferSize = lengthRead;
            }
            else
            {
                // Nothing left in buffer so just initialize xml buffer
                xmlBuffer[0] = '\0';
                xmlBufferSize = 0;
            }
        }
        else
        {
            // No ending found so just add what we received to the
            // internal collected xml buffer
            memcpy(&xmlBuffer[xmlBufferSize], buffer, lengthRead);
            xmlBufferSize += lengthRead;
        }
    }
}


void vsRemoteInterface::send(u_char *buffer, u_long bufferLen)
{
}

