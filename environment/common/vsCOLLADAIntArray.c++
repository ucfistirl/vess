
#include <stdio.h>
#include <string.h>
#include "vsCOLLADAIntArray.h++"


// ------------------------------------------------------------------------
// Creates an int array from the given XML subtree.  This is assumed to
// come from a COLLADA document and "current" should be pointing to an
// int_array node
// ------------------------------------------------------------------------
vsCOLLADAIntArray::vsCOLLADAIntArray(atXMLDocument *doc,
                                     atXMLDocumentNodePtr current)
{
    char *attr;
    char *nodeText;
    char *dataStr;
    int  readCount;

    // Initialize the array to NULL
    dataArray = NULL;

    // if this isn't an "int_array" node, bail
    if ((doc == NULL) ||
        (strcmp(doc->getNodeName(current), "int_array") != 0))
    {
        printf("vsCOLLADAIntArray::vsCOLLADAIntArray:\n");
        printf("   Document not valid, or not an int_array!\n");
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
        dataArray = (int *)calloc(dataCount, sizeof(int));

        // Get the text from the child of this node.  This will be the
        // array of values
        nodeText = strdup(doc->getNodeText(doc->getNextChildNode(current)));

        // Parse the data array from the text
        readCount = 0;
        dataStr = strtok(nodeText, " \n\r\t");
        while ((readCount < dataCount) && (dataStr != NULL))
        {
            // Store the item parsed from the text
            dataArray[readCount] = atoi(dataStr);

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
vsCOLLADAIntArray::~vsCOLLADAIntArray()
{
    // Clean up the data array
    if (dataArray != NULL)
        free(dataArray);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAIntArray::getClassName()
{
    return "vsCOLLADAIntArray";
}

// ------------------------------------------------------------------------
// Return the data type stored by this array
// ------------------------------------------------------------------------
vsCOLLADADataArray::DataType vsCOLLADAIntArray::getDataType()
{
    return vsCOLLADADataArray::INT;
}

// ------------------------------------------------------------------------
// Return the data element at the given index
// ------------------------------------------------------------------------
int vsCOLLADAIntArray::getData(int index)
{
    // If the index is valid, return the data at that index, otherwise
    // return a zero value
    if ((index >= 0) && (index < dataCount))
        return dataArray[index];
    else
        return 0;
}

