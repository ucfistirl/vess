
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
    int  readCount;
    int  idx;

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
        nodeText = doc->getNodeText(doc->getNextChildNode(current));
        idx = 0;

        // Parse the data array from the text
        readCount = 0;
        while ((readCount < dataCount) && (idx >= 0))
        {
            // Store the item parsed from the text
            dataArray[readCount] = getFloatToken(nodeText, &idx);

            // Increment the count
            readCount++;
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
// Parses a float from the given string at the given index and updates the
// index to point to the next token
// ------------------------------------------------------------------------
double vsCOLLADAFloatArray::getFloatToken(char *tokenString, int *idx)
{
    char *src;
    char *delim;
    char token[32];
    int advance;
    double value;

    // Check for invalid index
    if (*idx < 0)
       return 0;

    // Get a pointer to the token string at the correct index
    src = &tokenString[*idx];

    // Find the next delimiter in the token string (or the end of the string)
    delim = strpbrk(src, " \n\r\t");
    if (delim == NULL)
        delim = src + strlen(src);

    // Copy the token
    strncpy(token, src, delim - src);
    token[delim - src] = 0;

    // Convert the token to a integer value
    value = atof(token);

    // Advance the index past the delimiters
    advance = strspn(delim, " \n\r\t");
    *idx += strlen(token) + advance;

    // Check for end of string (we indicate this by setting idx to -1)
    if (tokenString[*idx] == 0)
       *idx = -1;

    // Return the double value
    return value;
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

