
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
    int  readCount;
    int  idx;

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
        nodeText = doc->getNodeText(doc->getNextChildNode(current));

        // Parse the data array from the text
        readCount = 0;
        idx = 0;
        while ((readCount < dataCount) && (idx >= 0))
        {
            // Store the next value
            dataArray[readCount] = getBoolToken(nodeText, &idx);

            // Increment the count
            readCount++;
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
// Parse a boolean token from the given string at the given index and 
// return it.  Update the index to point to the next token in the string
// ------------------------------------------------------------------------
bool vsCOLLADABoolArray::getBoolToken(char *tokenString, int *idx)
{
    char *src;
    char *delim;
    char token[32];
    int advance;
    bool value;

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

    // Convert the token to a bool
    // NOTE:  According to the COLLADA schema, only "true", "1",
    //        "false", and "0" are valid values
    if ((strcmp(token, "true") == 0) ||
        (strcmp(token, "1") == 0))
    {
        // It's a "true"
        value = true;
    }  
    else if ((strcmp(token, "false") == 0) ||
             (strcmp(token, "0") == 0))
    {
        // It's a "false"
        value = false;
    }
    else
    {
        // Invalid value.  Print a warning, but assume false
        printf("vsCOLLADABoolArray::parseBoolToken:\n");
        printf("    Invalid boolean value '%s', assuming false\n",
            token);
        value = false;
    }

    // Advance the index past the delimiters
    advance = strspn(delim, " \n\r\t");
    *idx += strlen(token) + advance;

    // Check for end of string (we indicate this by setting idx to -1)
    if (tokenString[*idx] == 0)
       *idx = -1;

    // Return the boolean value
    return value;
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

