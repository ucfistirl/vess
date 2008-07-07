
#include "vsCOLLADADataArray.h++"


// ------------------------------------------------------------------------
// Constructor for the base data array class.  Child classes will set up
// the actual data array and supporting data
// ------------------------------------------------------------------------
vsCOLLADADataArray::vsCOLLADADataArray()
{
    // Initialize the data count to zero for now
    dataCount = 0;
}

// ------------------------------------------------------------------------
// Destructor for the base data array class.  Child classes will deallocate
// the data array and perform cleanup
// ------------------------------------------------------------------------
vsCOLLADADataArray::~vsCOLLADADataArray()
{
}

// ------------------------------------------------------------------------
// Return this class's name
// ------------------------------------------------------------------------
const char *vsCOLLADADataArray::getClassName()
{
    return "vsCOLLADADataArray";
}

// ------------------------------------------------------------------------
// Return the ID of this data array
// ------------------------------------------------------------------------
atString vsCOLLADADataArray::getID()
{
    return dataID;
}

// ------------------------------------------------------------------------
// Return the number of data elements in this array
// ------------------------------------------------------------------------
int vsCOLLADADataArray::getDataCount()
{
    return dataCount;
}
