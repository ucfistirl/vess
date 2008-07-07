
#include <stdio.h>
#include <string.h>
#include "vsCOLLADABoolArray.h++"


// ------------------------------------------------------------------------
// Creates a bool array from the given XML subtree.  This is assumed to
// come from a COLLADA document and "current" should be pointing to an
// int_array node
// ------------------------------------------------------------------------
vsCOLLADABoolArray::vsCOLLADABoolArray(atXMLDocument *doc,
                                     atXMLDocumentNodePtr current)
{
    char *attr;
    char *nodeText;
    char *dataStr;
    int  readCount;

    // Initialize the array to NULL
    dataArray = NULL;

    // if this isn't an "bool_array" node, bail
    if ((doc == NULL) ||
        (strcmp(doc->getNodeName(current), "bool_array") != 0))
    {
        printf("vsCOLLADABoolArray::vsCOLLADABoolArray:\n");
        printf("   Document not valid, or not a bool_array!\n");
    }
    else
    {
        // Read and set the ID (if any)
        attr = doc->getNodeAttribute(current, "id");
        if (attr != NULL)
            dataID.setString(attr);

        // Read the number of elements
        dataCount = atoi(doc->getNodeAttribute(current, "count"));

        // Allocate the data array
        dataArray = (bool *)calloc(dataCount, sizeof(bool));

        // Get the text from the child of this node.  This will be the
        // array of values
        nodeText = strdup(doc->getNodeText(doc->getNextChildNode(current)));

        // Parse the data array from the text
        readCount = 0;
        dataStr = strtok(nodeText, " \n\r\t");
        while ((readCount < dataCount) && (dataStr != NULL))
        {
            // Convert the text entry into a boolean entry.
            // NOTE:  According to the COLLADA schema, only "true", "1",
            //        "false", and "0" are valid values
            if ((strcmp(dataStr, "true") == 0) ||
                (strcmp(dataStr, "1") == 0))
            {
                // Store a "true"
                dataArray[readCount] = true;
            }
            else if ((strcmp(dataStr, "false") == 0) ||
                     (strcmp(dataStr, "0") == 0))
            {
                // Store a "false"
                dataArray[readCount] = false;
            }
            else
            {
                // Invalid value.  Print a warning, but assume false
                printf("vsCOLLADABoolArray::vsCOLLADABoolArray:\n");
                printf("    Invalid boolean value '%s', assuming false\n",
                    dataStr);
                dataArray[readCount] = false;
            }

            // Increment the count
            readCount++;

            // Try to parse the next item
            dataStr = strtok(NULL, " \n\r\t");
        }
    }
}

// ------------------------------------------------------------------------
// Clean up the array
// ------------------------------------------------------------------------
vsCOLLADABoolArray::~vsCOLLADABoolArray()
{
    // Clean up the data array
    if (dataArray != NULL)
        free(dataArray);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADABoolArray::getClassName()
{
    return "vsCOLLADABoolArray";
}

// ------------------------------------------------------------------------
// Return the data type stored by this array
// ------------------------------------------------------------------------
vsCOLLADADataArray::DataType vsCOLLADABoolArray::getDataType()
{
    return vsCOLLADADataArray::BOOL;
}

// ------------------------------------------------------------------------
// Return the data element at the given index
// ------------------------------------------------------------------------
bool vsCOLLADABoolArray::getData(int index)
{
    // If the index is valid, return the data at that index, otherwise
    // return a zero value
    if ((index >= 0) && (index < dataCount))
        return dataArray[index];
    else
        return 0.0;
}

