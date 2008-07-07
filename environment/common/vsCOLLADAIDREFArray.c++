
#include <stdio.h>
#include <string.h>
#include "vsCOLLADAIDREFArray.h++"
#include "atStringTokenizer.h++"

// ------------------------------------------------------------------------
// Creates an IDREF array from the given XML subtree.  This is assumed to
// come from a COLLADA document and "current" should be pointing to a 
// IDREF_array node
// ------------------------------------------------------------------------
vsCOLLADAIDREFArray::vsCOLLADAIDREFArray(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current)
{
    char *attr;
    atString nodeText;
    atStringTokenizer *tokens;
    atString *idref;
    int readCount;

    // Initialize the array to NULL
    dataArray = NULL;

    // If this isn't a "IDREF_array" node, bail
    if ((doc == NULL) ||
        (strcmp(doc->getNodeName(current), "IDREF_array") != 0))
    {
        printf("vsCOLLADAIDREFArray::vsCOLLADAIDREFArray:\n");
        printf("   Document not valid, or not a IDREF_array!\n");
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
        // array of IDREF's
        nodeText.setString(doc->getNodeText(doc->getNextChildNode(current)));

        // Parse the data array from the text
        readCount = 0;
        tokens = new atStringTokenizer(nodeText);
        idref = tokens->getToken(" \n\r\t");       
        while ((readCount < dataCount) && (idref != NULL))
        {
            // Store the item parsed from the text
            dataArray->setEntry(readCount, idref);

            // Increment the count
            readCount++;

            // Try to parse the next item
            idref = tokens->getToken(" \n\r\t");
        }
    }
}

// ------------------------------------------------------------------------
// Clean up the array
// ------------------------------------------------------------------------
vsCOLLADAIDREFArray::~vsCOLLADAIDREFArray()
{
    // Clean up the data array
    if (dataArray != NULL)
        delete dataArray;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAIDREFArray::getClassName()
{
    return "vsCOLLADAIDREFArray";
}

// ------------------------------------------------------------------------
// Return the data type stored by this array
// ------------------------------------------------------------------------
vsCOLLADADataArray::DataType vsCOLLADAIDREFArray::getDataType()
{
    return vsCOLLADADataArray::IDREF;
}

// ------------------------------------------------------------------------
// Return the data element at the given index
// ------------------------------------------------------------------------
atString vsCOLLADAIDREFArray::getData(int index)
{
    // If the index is valid, return a clone of the idref at that index,
    // otherwise return an empty string
    if ((index >= 0) && (index < dataCount))
        return ((atString *)dataArray->getEntry(index))->clone();
    else
        return atString();
}

