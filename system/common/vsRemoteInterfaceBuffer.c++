
#include <string.h>
#include "vsRemoteInterfaceBuffer.h++"
#include "vsScreen.h++"
#include "vsWindow.h++"
#include "vsPane.h++"
#include "vsSystem.h++"
#include "vsSequencer.h++"
#include "vsTransformAttribute.h++"
#include "vsKinematics.h++"


#define   VS_VESS_XML_VERSION   "1.0"


// ------------------------------------------------------------------------
// Constructor - Initializes variables (we don't use a DTD)
// ------------------------------------------------------------------------
vsRemoteInterfaceBuffer::vsRemoteInterfaceBuffer()
{
    // Initialize the xml DTD (we aren't using one in this case and also
    // don't need a context then)
    xmlDTD = NULL;
}


// ------------------------------------------------------------------------
// Constructor - Initializes variables (including the DTD file with which
// to validate the XML documents against)
// ------------------------------------------------------------------------
vsRemoteInterfaceBuffer::vsRemoteInterfaceBuffer(char *dtdFilename)
{
    // Open and initialize the DTD file (used for validating XML)
    xmlDTD = xmlParseDTD(NULL, (const xmlChar *) dtdFilename);

    // Initialize the XML context information (used during XML validation
    // to print warnings and errors)
    xmlContext.userData = stderr;
    xmlContext.error = (xmlValidityErrorFunc ) fprintf;
    xmlContext.warning = (xmlValidityWarningFunc ) fprintf;
}


// ------------------------------------------------------------------------
// Destructor - Closes up the TCP network interface and frees the memory
// used by the DTD file
// ------------------------------------------------------------------------
vsRemoteInterfaceBuffer::~vsRemoteInterfaceBuffer()
{
   // Free up the DTD if we created it
   if (xmlDTD != NULL)
      xmlFreeDtd(xmlDTD);
}


// ------------------------------------------------------------------------
// Gets the position information at this level in the XML document
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::getPosition(xmlDocPtr doc, xmlNodePtr current,
                                          double *x, double *y, double *z)
{
    // Go through the childen and see what updates are there
    current = current->xmlChildrenNode;
    while (current != NULL)
    {
        // Look for element tags we recognize
        if (xmlStrcmp(current->name, (const xmlChar *) "x") == 0)
        {
            // Get the value of the "x" element and store it
            *x = atof((char *) xmlNodeListGetString(doc, 
                                                    current->xmlChildrenNode, 
                                                    1));
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "y") == 0)
        {
            // Get the value of the "y" element and store it
            *y = atof((char *) xmlNodeListGetString(doc, 
                                                    current->xmlChildrenNode, 
                                                    1));
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "z") == 0)
        {
            // Get the value of the "z" element and store it
            *z = atof((char *) xmlNodeListGetString(doc, 
                                                    current->xmlChildrenNode, 
                                                    1));
        }

        // Go to the next node
        current = current->next;
    }
}


// ------------------------------------------------------------------------
// Gets the orientation information at this level in the XML document
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::getOrientation(xmlDocPtr doc, xmlNodePtr current,
                                             vsQuat *quat)
{
    xmlChar                *order;
    vsMathEulerAxisOrder   ordering;
    double                 h;
    double                 p;
    double                 r;

    // Get the attribute for the axis ordering
    order = xmlGetProp(current, (const xmlChar *) "order");

    // Determine the proper VESS constant for the string
    if (xmlStrcmp(order, (const xmlChar *) "XYZ_S") == 0)
        ordering = VS_EULER_ANGLES_XYZ_S;
    else if (xmlStrcmp(order, (const xmlChar *) "XZY_S") == 0)
        ordering = VS_EULER_ANGLES_XZY_S;
    else if (xmlStrcmp(order, (const xmlChar *) "YXZ_S") == 0)
        ordering = VS_EULER_ANGLES_YXZ_S;
    else if (xmlStrcmp(order, (const xmlChar *) "YZX_S") == 0)
        ordering = VS_EULER_ANGLES_YZX_S;
    else if (xmlStrcmp(order, (const xmlChar *) "ZXY_S") == 0)
        ordering = VS_EULER_ANGLES_ZXY_S;
    else if (xmlStrcmp(order, (const xmlChar *) "ZYX_S") == 0)
        ordering = VS_EULER_ANGLES_ZYX_S;
    else if (xmlStrcmp(order, (const xmlChar *) "XYX_S") == 0)
        ordering = VS_EULER_ANGLES_XYX_S;
    else if (xmlStrcmp(order, (const xmlChar *) "XZX_S") == 0)
        ordering = VS_EULER_ANGLES_XZX_S;
    else if (xmlStrcmp(order, (const xmlChar *) "YXY_S") == 0)
        ordering = VS_EULER_ANGLES_YXY_S;
    else if (xmlStrcmp(order, (const xmlChar *) "YZY_S") == 0)
        ordering = VS_EULER_ANGLES_YZY_S;
    else if (xmlStrcmp(order, (const xmlChar *) "ZXZ_S") == 0)
        ordering = VS_EULER_ANGLES_ZXZ_S;
    else if (xmlStrcmp(order, (const xmlChar *) "ZYZ_S") == 0)
        ordering = VS_EULER_ANGLES_ZYZ_S;
    else if (xmlStrcmp(order, (const xmlChar *) "XYZ_R") == 0)
        ordering = VS_EULER_ANGLES_XYZ_R;
    else if (xmlStrcmp(order, (const xmlChar *) "XZY_R") == 0)
        ordering = VS_EULER_ANGLES_XZY_R;
    else if (xmlStrcmp(order, (const xmlChar *) "YXZ_R") == 0)
        ordering = VS_EULER_ANGLES_YXZ_R;
    else if (xmlStrcmp(order, (const xmlChar *) "YZX_R") == 0)
        ordering = VS_EULER_ANGLES_YZX_R;
    else if (xmlStrcmp(order, (const xmlChar *) "ZXY_R") == 0)
        ordering = VS_EULER_ANGLES_ZXY_R;
    else if (xmlStrcmp(order, (const xmlChar *) "ZYX_R") == 0)
        ordering = VS_EULER_ANGLES_ZYX_R;
    else if (xmlStrcmp(order, (const xmlChar *) "XYX_R") == 0)
        ordering = VS_EULER_ANGLES_XYX_R;
    else if (xmlStrcmp(order, (const xmlChar *) "XZX_R") == 0)
        ordering = VS_EULER_ANGLES_XZX_R;
    else if (xmlStrcmp(order, (const xmlChar *) "YXY_R") == 0)
        ordering = VS_EULER_ANGLES_YXY_R;
    else if (xmlStrcmp(order, (const xmlChar *) "YZY_R") == 0)
        ordering = VS_EULER_ANGLES_YZY_R;
    else if (xmlStrcmp(order, (const xmlChar *) "ZXZ_R") == 0)
        ordering = VS_EULER_ANGLES_ZXZ_R;
    else if (xmlStrcmp(order, (const xmlChar *) "ZYZ_R") == 0)
        ordering = VS_EULER_ANGLES_ZYZ_R;

    // Get euler angles from the passed in quaternion
    quat->getEulerRotation(ordering, &h, &p, &r);

    // Go through the children and see what updates are there
    current = current->xmlChildrenNode;
    while (current != NULL)
    {
        // Look for element tags we recognize
        if (xmlStrcmp(current->name, (const xmlChar *) "h") == 0)
        {
            // Store the new heading
            h = atof((char *) xmlNodeListGetString(doc, 
                                                   current->xmlChildrenNode, 
                                                   1));
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "p") == 0)
        {
            // Store the new pitch
            p = atof((char *) xmlNodeListGetString(doc, 
                                                   current->xmlChildrenNode, 
                                                   1));
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "r") == 0)
        {
            // Store the new roll
            r = atof((char *) xmlNodeListGetString(doc, 
                                                   current->xmlChildrenNode, 
                                                   1));
        }

        // Go to the next node
        current = current->next;
    }

    // Set the quaternion based on the new values
    quat->setEulerRotation(ordering, h, p, r);
}


// ------------------------------------------------------------------------
// Go through the sequence tree recursive and build up a representation
// of it
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::getSequenceTree(vsSequencer *currentSequencer,
                                              char *sequenceTreeBuffer)
{
    u_long         i;
    vsUpdatable    *updatable;
    char           updatableStr[255];

    // Loop through the updatables in this sequencer
    for (i=0; i < currentSequencer->getUpdatableCount(); i++)
    {
        // Get the ith updatable from this sequencer
        updatable = currentSequencer->getUpdatable(i);

        // If this updatable is a sequencer itself, call recursively
        // to process this new sequencer; otherwise, just add the
        // updatable to the built up structure
        if (strcmp(updatable->getClassName(), "vsSequencer") == 0)
        {
            // Add an hierarchy level to the XML document we're building up
            strcat(sequenceTreeBuffer, "<sequence>");

            // Go down in the tree sequence
            getSequenceTree((vsSequencer *) updatable, sequenceTreeBuffer);

            // Close the hierarchy level to the XML document we're building up
            strcat(sequenceTreeBuffer, "</sequence>");
        }
        else
        {
            // Add in the XML code for this updatable
            sprintf(updatableStr, 
                    "<updatable name=\"%s\" minlatency=\"%lf\"></updatable>",
                    currentSequencer->getUpdatableName(updatable),
                    currentSequencer->getUpdatableTime(updatable));
            strcat(sequenceTreeBuffer, updatableStr);
        }
    }
}


// ------------------------------------------------------------------------
// Takes an XML document from the ith xml buffer, validates it, determines
// what type it is, and calls the appropriate processing method
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processXMLDocument()
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
        if (xmlStrcmp(current->name, (const xmlChar *) "placecomponent") == 0)
        {
            processPlaceComponent(doc, current);
        }
        else if (xmlStrcmp(current->name, 
                           (const xmlChar *) "querysequence") == 0)
        {
            processQuerySequence(doc, current);
        }
        else if (xmlStrcmp(current->name, 
                           (const xmlChar *) "setkinematics") == 0)
        {
            processSetKinematics(doc, current);
        }
        else if (xmlStrcmp(current->name, 
                           (const xmlChar *) "setsequence") == 0)
        {
            processSetSequence(doc, current, 
                               vsSystem::systemObject->getSequencer());
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "stats") == 0)
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


// ------------------------------------------------------------------------
// Update the position and orientation (if given) of the given component
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processPlaceComponent(xmlDocPtr doc, 
                                                    xmlNodePtr current)
{
    int                    screenIndex;
    int                    windowIndex;
    int                    paneIndex;
    char                   *name;
    vsNode                 *node;
    vsTransformAttribute   *xform;
    vsMatrix               mat;
    double                 x;
    double                 y;
    double                 z;
    vsQuat                 quat;

    // Initialize the quaternion that stores the orientation
    quat.set(0, 0, 0, 1);

    // Look for the required attributes
    screenIndex = atoi((const char *) xmlGetProp(current, 
                                                 (const xmlChar *) "screen"));
    windowIndex = atoi((const char *) xmlGetProp(current, 
                                                 (const xmlChar *) "window"));
    paneIndex = atoi((const char *) xmlGetProp(current, 
                                               (const xmlChar *) "pane"));
    name = (char *) xmlGetProp(current, (const xmlChar *) "name");

    // Get the component named as given
    node = vsScreen::getScreen(screenIndex)->getChildWindow(windowIndex)->
               getChildPane(paneIndex)->getScene()->findNodeByName(name);

    // Get the transform on the node
    xform = (vsTransformAttribute *) node->getTypedAttribute(
        VS_ATTRIBUTE_TYPE_TRANSFORM, 0);

    // Check to make sure we got a transform; otherwise, make one and attach
    // it to the node
    if (xform == NULL)
    {
        // Create a new transform attribute and add it to the node that
        // was found
        xform = new vsTransformAttribute();
        node->addAttribute(xform);
    }

    // Get the matrix from the transform and get the position and orientation
    // out of it
    mat = xform->getDynamicTransform();
    mat.getTranslation(&x, &y, &z);
    quat.setMatrixRotation(mat);

    // Go through the children and see what updates are there
    current = current->xmlChildrenNode;
    while (current != NULL)
    {
        if (xmlStrcmp(current->name, (const xmlChar *) "position") == 0)
        {
            // Found a position so get the components out of it
            getPosition(doc, current, &x, &y, &z);
        }
        else if (xmlStrcmp(current->name, (const xmlChar *) "orientation") == 0)
        {
            // Found an orientation so get the components out of it
            getOrientation(doc, current, &quat);
        }

        // Get the next child of the root element
        current = current->next;
    }

    // Set the new transform onto the transform attribute
    mat.setTranslation(x, y, z);
    mat.setQuatRotation(quat);
    xform->setDynamicTransform(mat);
}


// ------------------------------------------------------------------------
// Get the current sequence from the "root" sequencer and send it back
// to the calling client
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processQuerySequence(xmlDocPtr doc, 
                                                   xmlNodePtr current)
{
    vsSequencer   *rootSequencer;
    char          treeBuffer[1024];

    // Get the "root" sequencer
    rootSequencer = vsSystem::systemObject->getSequencer();

    // Initialize the buffer for the response
    strcpy(treeBuffer, "<sequence>");

    // Go through and collect the state of the sequencer
    getSequenceTree(rootSequencer, treeBuffer);

    // Put the ending on the response XML 
    strcat(treeBuffer, "</sequence>");

    // Copy the tree buffer into the responses buffer that will be sent back
    strcat((char *) xmlResponses, (char *) treeBuffer);
}


// ------------------------------------------------------------------------
// Set a kinematics object (position and orientation)
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processSetKinematics(xmlDocPtr doc, 
                                                   xmlNodePtr current)
{
    vsQuat         quat;
    char           *kinematicsName;
    vsSequencer    *rootSequencer;
    vsKinematics   *kinematics;
    vsVector       positionVector;
    double         x;
    double         y;
    double         z;

    // Initialize the quaternion that stores the orientation
    quat.set(0, 0, 0, 1);

    // Look for the required attribute
    kinematicsName = (char *) xmlGetProp(current, (const xmlChar *) "name");

    // Search for the kinematics associated with the name in the sequencer
    rootSequencer = vsSystem::systemObject->getSequencer();
    kinematics = 
        (vsKinematics *) rootSequencer->getUpdatableByName(kinematicsName);

    // Make sure we found the kinematics we were looking for
    if (kinematics == NULL)
        printf("Kinematics not found from name in <setkinematics> element.\n");
    else
    {
        // Get the data out of the kinematics
        positionVector = kinematics->getPosition();
        x = positionVector[VS_X];
        y = positionVector[VS_Y];
        z = positionVector[VS_Z];
        quat = kinematics->getOrientation();

        // Go through the children and see what updates are there
        current = current->xmlChildrenNode;
        while (current != NULL)
        {
            // Process the subelements within the setkinematics call
            if (xmlStrcmp(current->name, (const xmlChar *) "position") == 0)
            {
                getPosition(doc, current, &x, &y, &z);
            }
            else if (xmlStrcmp(current->name, 
                               (const xmlChar *) "orientation") == 0)
            {
                getOrientation(doc, current, &quat);
            }
            else if (xmlStrcmp(current->name, 
                               (const xmlChar *) "linearvelocity") == 0)
            {
            }
            else if (xmlStrcmp(current->name, 
                               (const xmlChar *) "angularvelocity") == 0)
            {
            }
            else if (xmlStrcmp(current->name, 
                               (const xmlChar *) "centerofmass") == 0)
            {
            }

            // Get the next child of the root element
            current = current->next;
        }

        // Set the kinematics with the new data
        positionVector[VS_X] = x;
        positionVector[VS_Y] = y;
        positionVector[VS_Z] = z;
        kinematics->setPosition(positionVector);
        kinematics->setOrientation(quat);
    }
}


// ------------------------------------------------------------------------
// Set the root sequencer to the sequence listed in the XML document
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processSetSequence(xmlDocPtr doc, 
                                                 xmlNodePtr current,
                                                 vsSequencer *currentSequencer)
{
    u_long         numUpdatable;
    char           *updatableName;
    char           *updatableLatency;
    vsUpdatable    *updatable;

    // Initialize count
    numUpdatable = 0;

    // Go through the children and see what updatables are there
    current = current->xmlChildrenNode;
    while (current != NULL)
    {
        // Process accordingly whether it's an "updatable" or a "sequence"
        // Note that updatables not in the XML list will be moved to the
        // end of the list (not deleted)
        if ( (xmlStrcmp(current->name, (const xmlChar *) "updatable") == 0) ||
             (xmlStrcmp(current->name, (const xmlChar *) "sequence") == 0) )
        {
            // Look for the required attributes
            updatableName = (char *) xmlGetProp(current, 
                                                (const xmlChar *) "name");
            updatableLatency = (char *) xmlGetProp(current, 
                                           (const xmlChar *) "minlatency");

            // Find the updatable in the sequencer's list
            updatable = currentSequencer->getUpdatableByName(updatableName);

            // Set the position of this updatable (assuming we found it)
            if (updatable != NULL)
            {
                // Place the updatable and the relative position we want
                currentSequencer->setUpdatablePosition(updatable, numUpdatable);

                // Set the latency on the updatable (but not sequences)
                if (updatableLatency != NULL)
                {
                    currentSequencer->setUpdatableTime(updatable, 
                                                       atof(updatableLatency));
                }

                // Move to the next relative position
                numUpdatable++;
            }

            // Then, see if this updatable is actually a sequence (hierarchical)
            if (xmlStrcmp(current->name, (const xmlChar *) "sequence") == 0)
            {
                // It is so call recursively
                processSetSequence(doc, current, (vsSequencer *) updatable);
            }
        }

        // Get the next child of the element
        current = current->next;
    }
}


// ------------------------------------------------------------------------
// Get the data out of the showstats XML document and enable/disable
// the display of stats on that pane accordingly
// ------------------------------------------------------------------------
void vsRemoteInterfaceBuffer::processStats(xmlDocPtr doc, xmlNodePtr current)
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


// ------------------------------------------------------------------------
// Take in a buffer and process it by adding to the local XML buffer
// and seeing if it has a complete XML document
// ------------------------------------------------------------------------
u_char *vsRemoteInterfaceBuffer::processBuffer(u_char *buffer, int lengthRead)
{
    u_char   *nextNull;
    u_char   *endTag;
    int      partialChunkSize;
    int      numWhiteSpace;
    char     header[255];

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
    // end tag "</vessxml>" gets split between packets)
    memmove(&buffer[xmlBufferSize], &buffer[0], lengthRead);
    memcpy(&buffer[0], &xmlBuffer[0], xmlBufferSize);
    lengthRead += xmlBufferSize;
    xmlBufferSize = 0;

    // Make a string out of the buffer
    buffer[lengthRead] = '\0';

    // Initialize the responses buffer (we will collect any responses
    // from the XML elements into this buffer and send them back)
    xmlResponses[0] = '\0';

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

    // Add on the XML header and footer to the responses (if we have responses)
    if (strlen((char *) xmlResponses) > 0)
    {
        // Create the xml header info
        sprintf(header, "<?xml version=\"1.0\"?><vessxml version=\"%s\">",
                VS_VESS_XML_VERSION);

        // Move the responses over to make room for the header (+1 for NULL)
        memmove(&xmlResponses[strlen(header)], &xmlResponses[0], 
                strlen((char *) xmlResponses)+1);

        // Copy the header in
        memcpy(&xmlResponses[0], header, strlen(header));

        // Add the xml footer
        strcat((char *) xmlResponses, "</vessxml>");
    }

    // Return the responses that were collected
    return xmlResponses;
}

