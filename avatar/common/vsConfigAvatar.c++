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
//    VESS Module:  vsConfigAvatar.c++
//
//    Description:  Avatar subclass that operates completely off of the
//                  data within a configuration file; no subclassing of
//                  this class should be required.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsConfigAvatar.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsConfigAvatar::vsConfigAvatar() : vsAvatar()
{
}

// ------------------------------------------------------------------------
// Constructor
// Passes the specified scene graph through to the parent class'
// constructor
// ------------------------------------------------------------------------
vsConfigAvatar::vsConfigAvatar(vsNode *scene) : vsAvatar(scene)
{
}

// ------------------------------------------------------------------------
// Destructor
// Destroys the contents of the avatar's updatable object list
// ------------------------------------------------------------------------
vsConfigAvatar::~vsConfigAvatar()
{
    // Delete any geometry loaded by this avatar
    if (geometryRoot)
    {
        geometryRoot->deleteTree();
        delete geometryRoot;
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsConfigAvatar::getClassName()
{
    return "vsConfigAvatar";
}

// ------------------------------------------------------------------------
// Updates this avatar by calling update on every object in its list
// ------------------------------------------------------------------------
void vsConfigAvatar::update()
{
    int loop;
    
    // Send an update call to each object in the update list
    for (loop = 0; loop < updateList.getNumEntries(); loop++)
        if (updateList.getEntry(loop) != NULL)
            ((vsUpdatable *)(updateList.getEntry(loop)))->update();
}

// ------------------------------------------------------------------------
// Sets this avatar up by building the list of objects owned by the avatar
// that need to be updated each frame. This list is mostly the list of
// objects created by the configuration file reader, but with
// non-updatable objects removed, and with vsKinematics objects moved to
// the end of the list. (vsKinematics objects must be updated last because
// they depend on data generated by the other objects' update functions,
// but they're not specified last in the config file; motion models that
// use the kinematics objects must appear later in the file.)
// ------------------------------------------------------------------------
void vsConfigAvatar::setup()
{
    vsArray kinArray;
    int loop;
    char *objType;

    // If we're not currently initializing the avatar, abort
    if (!objectArray)
        return;

    // For each object, determine what it is from the type string.
    // vsKinematics objects are stored in a temporary array so they can be
    // added onto the update list later. A scene graph (specified by the
    // "geometry" type) is stored in the parent class' geometryRoot variable
    // for later use. All other types that start with "vs" are assumed to
    // be subclasses of vsUpdatable and are added directly to the update
    // list. Everything else is ignored.
    for (loop = 0; loop < objectCount; loop++)
    {
        objType = ((atString *)objTypeArray->getEntry(loop))->getString();
        if (!strcmp(objType, "vsKinematics"))
            kinArray.addEntry(objectArray->getEntry(loop));
        else if (!strcmp(objType, "geometry"))
            geometryRoot = (vsComponent *)(objectArray->getEntry(loop));
        else if (!strncmp(objType, "vs", 2))
            updateList.addEntry(objectArray->getEntry(loop));
    }
    
    // Add the stored vsKinematics objects to the end of the update list
    for (loop = 0; loop < kinArray.getNumEntries(); loop++)
        updateList.addEntry(kinArray.getEntry(loop));
}
