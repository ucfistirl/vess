
#include <stdio.h>
#include <string.h>
#include "vsCOLLADANameArray.h++"
#include "atStringTokenizer.h++"

// ------------------------------------------------------------------------
// Creates a name array from the given XML subtree.  This is assumed to
// come from a COLLADA document and "current" should be pointing to a 
// Name_array node
// ------------------------------------------------------------------------
vsCOLLADANameArray::vsCOLLADANameArray(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current)
{
    char *attr;
    atString nodeText;
    atStringTokenizer *tokens;
    atString *name;
    int readCount;

    // Initialize the array to NULL
    dataArray = NULL;

    // If this isn't a "Name_array" node, bail
    if ((doc == NULL) ||
        (strcmp(doc->getNodeName(current), "Name_array") != 0))
    {
        printf("vsCOLLADANameArray::vsCOLLADANameArray:\n");
        printf("   Document not valid, or not a Name_array!\n");
    }
    else
    {
        // Read and set the ID (if any)
        attr = doc->getNodeAttribute(current, "id");
        if (attr != NULL)
            dataID.setString(attr);

        // Read the number of elements
        dataCount = atoi(doc->getNodeAttribute(current, "count"));

        // Create the array
        dataArray = new atArray();

        // Get the text from the child of this node.  This will be the
        // array of names
        nodeText.setString(doc->getNodeText(doc->getNextChildNode(current)));

        // Parse the data array from the text
        readCount = 0;
        tokens = new atStringTokenizer(nodeText);
        name = tokens->getToken(" \n\r\t");       
        while ((readCount < dataCount) && (name != NULL))
        {
            // Store the item parsed from the text
            dataArray->setEntry(readCount, name);

            // Increment the count
            readCount++;

            // Try to parse the next item
            name = tokens->getToken(" \n\r\t");
        }

        // Done with the tokenizer
        delete tokens;
    }
}

// ------------------------------------------------------------------------
// Clean up the array
// ------------------------------------------------------------------------
vsCOLLADANameArray::~vsCOLLADANameArray()
{
    // Clean up the data array
    if (dataArray != NULL)
        delete dataArray;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADANameArray::getClassName()
{
    return "vsCOLLADANameArray";
}

// ------------------------------------------------------------------------
// Return the data type stored by this array
// ------------------------------------------------------------------------
vsCOLLADADataArray::DataType vsCOLLADANameArray::getDataType()
{
    return vsCOLLADADataArray::NAME;
}

// ------------------------------------------------------------------------
// Return the data element at the given index
// ------------------------------------------------------------------------
atString vsCOLLADANameArray::getData(int index)
{
    // If the index is valid, return a clone of the name at that index,
    // otherwise return an empty string
    if ((index >= 0) && (index < dataCount))
        return ((atString *)dataArray->getEntry(index))->clone();
    else
        return atString();
}

