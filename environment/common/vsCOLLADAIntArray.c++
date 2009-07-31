
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
    int  readCount;
    int  idx;

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
        nodeText = doc->getNodeText(doc->getNextChildNode(current));

        // Parse the data array from the text
        readCount = 0;
        idx = 0;
        while ((readCount < dataCount) && (idx >= 0))
        {
            // Store the item parsed from the text
            dataArray[readCount] = getIntToken(nodeText, &idx);

            // Increment the count
            readCount++;
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
// Parses an integer from the given string at the given index and updates
// the index to point to the next token in the string
// ------------------------------------------------------------------------
int vsCOLLADAIntArray::getIntToken(char *tokenString, int *idx)
{
    char *src;
    char *delim;
    char token[32];
    int advance;
    int value;

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
    value = atoi(token);

    // Advance the index past the delimiters
    advance = strspn(delim, " \n\r\t");
    *idx += strlen(token) + advance;

    // Check for end of string (we indicate this by setting idx to -1)
    if (tokenString[*idx] == 0)
       *idx = -1;

    // Return the integer value
    return value;
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

