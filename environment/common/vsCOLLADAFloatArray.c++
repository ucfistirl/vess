
#include <stdio.h>
#include <string.h>
#include "vsCOLLADAFloatArray.h++"


// ------------------------------------------------------------------------
// Creates a float array from the given XML subtree.  This is assumed to
// come from a COLLADA document and "current" should be pointing to a 
// float_array node
// ------------------------------------------------------------------------
vsCOLLADAFloatArray::vsCOLLADAFloatArray(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current)
{
    char *attr;
    char *nodeText;
    char *dataStr;
    int  readCount;

    // Initialize the array to NULL
    dataArray = NULL;

    // if this isn't a "float_array" node, bail
    if ((doc == NULL) ||
        (strcmp(doc->getNodeName(current), "float_array") != 0))
    {
        printf("vsCOLLADAFloatArray::vsCOLLADAFloatArray:\n");
        printf("   Document not valid, or not a float_array!\n");
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
        dataArray = (double *)calloc(dataCount, sizeof(double));

        // Get the text from the child of this node.  This will be the
        // array of values
        nodeText = strdup(doc->getNodeText(doc->getNextChildNode(current)));

        // Parse the data array from the text
        readCount = 0;
        dataStr = strtok(nodeText, " \n\r\t");
        while ((readCount < dataCount) && (dataStr != NULL))
        {
            // Store the item parsed from the text
            dataArray[readCount] = atof(dataStr);

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
vsCOLLADAFloatArray::~vsCOLLADAFloatArray()
{
    // Clean up the data array
    if (dataArray != NULL)
        free(dataArray);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAFloatArray::getClassName()
{
    return "vsCOLLADAFloatArray";
}

// ------------------------------------------------------------------------
// Return the data type stored by this array
// ------------------------------------------------------------------------
vsCOLLADADataArray::DataType vsCOLLADAFloatArray::getDataType()
{
    return vsCOLLADADataArray::FLOAT;
}

// ------------------------------------------------------------------------
// Return the data element at the given index
// ------------------------------------------------------------------------
double vsCOLLADAFloatArray::getData(int index)
{
    // If the index is valid, return the data at that index, otherwise
    // return a zero value
    if ((index >= 0) && (index < dataCount))
        return dataArray[index];
    else
        return 0.0;
}

