// File vsAttribute.c++

#include "vsAttribute.h++"

#include <string.h>
#include <Performer/pr/pfMemory.h>

// ------------------------------------------------------------------------
// Allocator - Creates memory for the object in Performer's shared memory
// space
// ------------------------------------------------------------------------
void *vsAttribute::operator new(size_t objSize)
{
    return (pfMemory::calloc(objSize, 1));
}

// ------------------------------------------------------------------------
// Deallocator - Destroys an object in Performer's shared memory space
// ------------------------------------------------------------------------
void vsAttribute::operator delete(void *deadObj)
{
    pfMemory::free(deadObj);
}

// ------------------------------------------------------------------------
// Constructor - Initializes the object
// ------------------------------------------------------------------------
vsAttribute::vsAttribute()
{
    attributeName[0] = 0;
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsAttribute::~vsAttribute()
{
}

// ------------------------------------------------------------------------
// Retrieves the value of the 'attached' flag for this attribute
// ------------------------------------------------------------------------
int vsAttribute::isAttached()
{
    return attachedFlag;
}

// ------------------------------------------------------------------------
// Sets the name of this attribute
// ------------------------------------------------------------------------
void vsAttribute::setName(char *newName)
{
    strncpy(attributeName, newName, VS_ATTRIBUTE_NAME_MAX_LENGTH);
    attributeName[VS_ATTRIBUTE_NAME_MAX_LENGTH - 1] = 0;
}

// ------------------------------------------------------------------------
// Retrieves the name of this attribute
// ------------------------------------------------------------------------
const char *vsAttribute::getName()
{
    return attributeName;
}

// ------------------------------------------------------------------------
// VESS internal function
// ------------------------------------------------------------------------
int vsAttribute::canAttach()
{
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Marks this attribute as attached
// ------------------------------------------------------------------------
void vsAttribute::attach(vsNode *theNode)
{
    attachedFlag++;
}

// ------------------------------------------------------------------------
// VESS internal function
// Removes an attachment mark from this attribute
// ------------------------------------------------------------------------
void vsAttribute::detach(vsNode *theNode)
{
    attachedFlag--;
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::saveCurrent()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::apply()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::restoreSaved()
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Empty virtual base function
// ------------------------------------------------------------------------
void vsAttribute::setState()
{
}
