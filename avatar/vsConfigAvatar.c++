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
vsConfigAvatar::vsConfigAvatar() : vsAvatar(), updateList(10, 10)
{
    updateListSize = 0;
}

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsConfigAvatar::vsConfigAvatar(vsComponent *scene) : vsAvatar(scene),
    updateList(10, 10)
{
    updateListSize = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsConfigAvatar::~vsConfigAvatar()
{
    int loop;
    
    for (loop = 0; loop < updateListSize; loop++)
        if (updateList[loop])
            delete ((vsUpdatable *)(updateList[loop]));
}

// ------------------------------------------------------------------------
// Updates this avatar by calling update on every object in its list
// ------------------------------------------------------------------------
void vsConfigAvatar::update()
{
    int loop;
    
    for (loop = 0; loop < updateListSize; loop++)
        if (updateList[loop])
            ((vsUpdatable *)(updateList[loop]))->update();
}

// ------------------------------------------------------------------------
// Sets this avatar up by building the list of objects owned by the avatar
// that need to be updated each frame. This list is mostly the list of
// objects created by the configuration file reader, but with
// non-updatable objects removed, and with vsKinematics objects moved to
// the end of the list.
// ------------------------------------------------------------------------
void vsConfigAvatar::setup()
{
    vsGrowableArray kinArray(10, 10);
    int kinArraySize = 0;
    int loop;

    if (!objectArray)
        return;

    for (loop = 0; loop < objectCount; loop++)
    {
//        printf("object: %p %s %s\n", objectArray->getData(loop),
//            (char *)(objTypeArray->getData(loop)), 
//            (char *)(objNameArray->getData(loop)));
        if (!strcmp((char *)(objTypeArray->getData(loop)), "vsKinematics"))
            kinArray[kinArraySize++] = objectArray->getData(loop);
        else if (!strcmp((char *)(objTypeArray->getData(loop)), "geometry"))
            geometryRoot = (vsComponent *)(objectArray->getData(loop));
        else if (!strncmp((char *)(objTypeArray->getData(loop)), "vs", 2))
            updateList[updateListSize++] = objectArray->getData(loop);
    }
    
    for (loop = 0; loop < kinArraySize; loop++)
        updateList[updateListSize++] = kinArray[loop];
}
