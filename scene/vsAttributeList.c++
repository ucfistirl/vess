// File vsAttributeList.c++

#include "vsAttributeList.h++"

// ------------------------------------------------------------------------
// Allocator - Creates memory for the object in Performer's shared memory
// space
// ------------------------------------------------------------------------
void *vsAttributeList::operator new(size_t objSize)
{
    return (pfMemory::calloc(objSize, 1));
}

// ------------------------------------------------------------------------
// Deallocator - Destroys an object in Performer's shared memory space
// ------------------------------------------------------------------------
void vsAttributeList::operator delete(void *deadObj)
{
    pfMemory::free(deadObj);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the attribute list
// ------------------------------------------------------------------------
vsAttributeList::vsAttributeList()
{
    attributeCount = 0;
}

// ------------------------------------------------------------------------
// Destructor - Deletes each attribute owned by the list
// ------------------------------------------------------------------------
vsAttributeList::~vsAttributeList()
{
    int loop;
    
    for (loop = 0; loop < attributeCount; loop++)
        delete attributeList[loop];
}

// ------------------------------------------------------------------------
// Adds the specified attribute to the list
// ------------------------------------------------------------------------
void vsAttributeList::addAttribute(vsAttribute *newAttribute)
{
    if (attributeCount >= VS_ATTRIBUTE_LIST_SIZE)
    {
        printf("vsAttributeList::addAttribute: Attribute list is full\n");
        return;
    }

    attributeList[attributeCount] = newAttribute;
    attributeCount++;
}

// ------------------------------------------------------------------------
// Removes the specified attribute from the list
// ------------------------------------------------------------------------
void vsAttributeList::removeAttribute(vsAttribute *targetAttribute)
{
    int loop, sloop;

    for (loop = 0; loop < attributeCount; loop++)
        if (attributeList[loop] == targetAttribute)
        {
            for (sloop = loop; sloop < attributeCount-1; sloop++)
                attributeList[sloop] = attributeList[sloop+1];
            attributeCount--;
            return;
        }
}

// ------------------------------------------------------------------------
// Retrieves the number of attributes currently in this list
// ------------------------------------------------------------------------
int vsAttributeList::getAttributeCount()
{
    return attributeCount;
}

// ------------------------------------------------------------------------
// Retrieves the attribute specified by index from the list. The index of
// the first attribute is 0.
// ------------------------------------------------------------------------
vsAttribute *vsAttributeList::getAttribute(int index)
{
    if ((index < 0) || (index >= attributeCount))
    {
        printf("vsAttributeList::getAttribute: Index out of bounds\n");
        return NULL;
    }
    
    return attributeList[index];
}

// ------------------------------------------------------------------------
// Retrieves the attribute specified by the attribute type attribType and
// index from the list. The index of the first attribute of the given
// type in the list is 0.
// ------------------------------------------------------------------------
vsAttribute *vsAttributeList::getTypedAttribute(int attribType, int index)
{
    int loop, count;
    
    count = 0;
    for (loop = 0; loop < attributeCount; loop++)
        if (attribType == (attributeList[loop])->getAttributeType())
        {
            if (index == count)
                return attributeList[loop];
            else
                count++;
        }

    return NULL;
}

// ------------------------------------------------------------------------
// Retrieves the attribute with the given name from the list
// ------------------------------------------------------------------------
vsAttribute *vsAttributeList::getNamedAttribute(char *attribName)
{
    int loop;
    
    for (loop = 0; loop < attributeCount; loop++)
        if (!strcmp(attribName, attributeList[loop]->getName()))
            return attributeList[loop];
    
    return NULL;
}
