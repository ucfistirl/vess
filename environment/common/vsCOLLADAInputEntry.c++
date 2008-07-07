
#include "vsCOLLADAInputEntry.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsCOLLADAInputEntry::vsCOLLADAInputEntry(vsCOLLADADataSource *src, int list,
                                         int offset)
{
    // Copy the parameters
    dataSource = src;
    geometryDataList = list;
    inputOffset = offset;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCOLLADAInputEntry::~vsCOLLADAInputEntry()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAInputEntry::getClassName()
{
    return "vsCOLLADAInputEntry";
}

// ------------------------------------------------------------------------
// Clones this InputEntry object
// ------------------------------------------------------------------------
vsCOLLADAInputEntry *vsCOLLADAInputEntry::clone()
{   
    return new vsCOLLADAInputEntry(dataSource, geometryDataList, inputOffset);
}   
    
// ------------------------------------------------------------------------
// Returns the data source for this input entry
// ------------------------------------------------------------------------
vsCOLLADADataSource *vsCOLLADAInputEntry::getSource()
{   
    return dataSource;
}

// ------------------------------------------------------------------------
// Returns the intended vsGeometry data list for this input entry
// ------------------------------------------------------------------------
int vsCOLLADAInputEntry::getDataList()
{
    return geometryDataList;
}

// ------------------------------------------------------------------------
// Returns the index offset for this input entry
// ------------------------------------------------------------------------
int vsCOLLADAInputEntry::getOffset()
{
    return inputOffset;
}

