//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsObject.c++
//
//    Description:  Reference counting and object validation base class
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsGlobals.h++"

#include "vsObject.h++"

//------------------------------------------------------------------------
// Constructor - Initializes the magic number and reference count
//------------------------------------------------------------------------
vsObject::vsObject()
{
    magicNumber = VS_OBJ_MAGIC_NUMBER;
    refCount = 0;
}

//------------------------------------------------------------------------
// Destructor - Complains if the object being deleted is invalid or
// still referenced
//------------------------------------------------------------------------
vsObject::~vsObject()
{
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
        printf("vsObject::~vsObject: Deletion of invalid object\n");
    else if (refCount != 0)
        printf("vsObject::~vsObject: Deletion of referenced object\n");

    magicNumber = 0;
}

//------------------------------------------------------------------------
// Informs this object that it is being used by another
//------------------------------------------------------------------------
void vsObject::ref()
{
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
    {
        printf("vsObject::ref: Operation on invalid object\n");
        return;
    }
    
    refCount++;
}

//------------------------------------------------------------------------
// Informs this object that it is no longer being used by another
//------------------------------------------------------------------------
void vsObject::unref()
{
    if (magicNumber != VS_OBJ_MAGIC_NUMBER)
    {
        printf("vsObject::unref: Operation on invalid object\n");
        return;
    }
    if (refCount < 1)
    {
        printf("vsObject::unref: Called on unreferenced object\n");
        return;
    }
    
    refCount--;
}

//------------------------------------------------------------------------
// Returns the number of objects using this one
//------------------------------------------------------------------------
int vsObject::getRefCount()
{
    return refCount;
}

//------------------------------------------------------------------------
// Checks the value of the magic number and returns true if it is what it
// should be or false if not
//------------------------------------------------------------------------
int vsObject::isValidObject()
{
    if (magicNumber == VS_OBJ_MAGIC_NUMBER)
        return VS_TRUE;
    return VS_FALSE;
}
