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
//    VESS Module:  vsSoundListenerAttribute.c++
//
//    Description:  Attribute to maintain the location/orientation of the
//                  listener in the VESS scene graph
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <AL/al.h>
#include <stdio.h>

#include "vsSoundManager.h++"
#include "vsSoundListenerAttribute.h++"
#include "vsNode.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructor - Registers this attribute with the specified view object,
// and initializes the adjustment matrix.
// ------------------------------------------------------------------------
vsSoundListenerAttribute::vsSoundListenerAttribute()
{
    ALfloat zero[6];

    // Do some initialization
    offsetMatrix.setIdentity();
    parentComponent = NULL;

    // Initialize the vector holding the last position of the listener
    lastPos.set(0.0, 0.0, 0.0);
    lastOrn.set(0.0, 0.0, 0.0, 1.0);

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Set up a buffer of six zero-valued floats, used to initialize
    // the alListener below.
    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;
    zero[3] = 0.0;
    zero[4] = 0.0;
    zero[5] = 0.0;

    // Configure the listener
    alListenerfv(AL_POSITION, zero);
    alListenerfv(AL_ORIENTATION, zero);
    alListenerfv(AL_VELOCITY, zero);

    // Register with the sound manager
    vsSoundManager::getInstance()->setSoundListener(this);
}

// ------------------------------------------------------------------------
// Destructor - Detatched this attribute from its associated view object
// ------------------------------------------------------------------------
vsSoundListenerAttribute::~vsSoundListenerAttribute()
{
    // Unregister from the sound manager
    vsSoundManager::getInstance()->removeSoundListener(this);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSoundListenerAttribute::getClassName()
{
    return "vsSoundListenerAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsSoundListenerAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SOUND_LISTENER;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsSoundListenerAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_CONTAINER;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::attach(vsNode *theNode)
{
    // Make sure we're not already attached
    if (attachedCount)
    {
        printf("vsSoundListenerAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    // Make sure the target node is a component.  Scene and geometry
    // nodes can't take this attribute.
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsSoundListenerAttribute::attach: Can only attach sound "
            "listener attributes to vsComponents\n");
        return;
    }

    // Save the component we're attaching to
    parentComponent = ((vsComponent *)theNode);
    
    // Flag this attribute as attached to a component
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::detach(vsNode *theNode)
{
    // Make sure we're actually attached to the node
    if (!attachedCount)
    {
        printf("vsSoundListenerAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Detach from the node
    parentComponent = NULL;

    // Flag this attribute as not attached to any component
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Intended to attach a duplicate of this attribute to the given node. This
// operation is not possible for this type of attribute because there can
// only be a single listener per application, and thus one container on the 
// tree.
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns the effective position of the listener, after applying global
// transform and offset matrices
// ------------------------------------------------------------------------
atVector vsSoundListenerAttribute::getLastPosition()
{
    return lastPos;
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns the effective orientation of the listener, after applying global
// transform and offset matrices
// ------------------------------------------------------------------------
atQuat vsSoundListenerAttribute::getLastOrientation()
{
    return lastOrn;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is sent to the 
// OpenAL sound source.
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::setOffsetMatrix(atMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
atMatrix vsSoundListenerAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to its associated alListener object.
// -----------------------------------------------------------------------
void vsSoundListenerAttribute::update()
{
    atMatrix       result;
    atQuat         tempQuat;
    atVector       tempVec;
    atVector       deltaVec;
    double         interval;
    atVector       atVec, upVec;
    ALfloat        orientation[6];

    // If the attribute is not attached to a component, we have nothing
    // to do.  Simply return
    if (!attachedCount)
        return;

    // Get the component's global transform
    result = parentComponent->getGlobalXform();

    // Apply the listener's offset matrix
    result = result * offsetMatrix;
    
    // Apply the VESS-to-OpenAL coordinate conversion
    tempVec.setSize(3);
    tempVec[AT_X] = result[0][3];
    tempVec[AT_Y] = result[1][3];
    tempVec[AT_Z] = result[2][3];
    tempVec = coordXform.rotatePoint(tempVec);

    // Update the OpenAL listener's position
    alListener3f(AL_POSITION, (float)tempVec[AT_X], (float)tempVec[AT_Y], 
        (float)tempVec[AT_Z]); 

    // Update the velocity (based on the last frame's position)
    deltaVec.setSize(3);
    deltaVec = tempVec - lastPos;
    interval = vsTimer::getSystemTimer()->getInterval();

    // Make sure time has passed before trying to compute the velocity
    // (otherwise a divide by zero would result)
    if (interval > 0.0)
    {
        // Divide the change in position by time to get a velocity vector
        deltaVec.scale(1/interval);

        // Update the listener's velocity
        alListener3f(AL_VELOCITY, (float)deltaVec[AT_X], (float)deltaVec[AT_Y], 
            (float)deltaVec[AT_Z]);

        // Save the current position for the next frame
        lastPos = tempVec;
    }

    // Update the orientation
    tempQuat.setMatrixRotation(result);
    tempQuat = coordXform * tempQuat * coordXformInv;

    // Convert the atQuat rotation to at/up vectors
    // In OpenAL (like OpenGL) -Z is forward (at) and +Y is up
    atVec.set(0.0, 0.0, -1.0);
    upVec.set(0.0, 1.0, 0.0);

    // Save the orientation, in case someone (like the sound manager) 
    // needs it
    lastOrn = tempQuat;

    // Rotate the vectors by tempQuat
    atVec = tempQuat.rotatePoint(atVec);
    upVec = tempQuat.rotatePoint(upVec);

    // Update the alListener's orientation
    orientation[0] = (ALfloat)(atVec[AT_X]);
    orientation[1] = (ALfloat)(atVec[AT_Y]);
    orientation[2] = (ALfloat)(atVec[AT_Z]);
    orientation[3] = (ALfloat)(upVec[AT_X]);
    orientation[4] = (ALfloat)(upVec[AT_Y]);
    orientation[5] = (ALfloat)(upVec[AT_Z]);
    alListenerfv(AL_ORIENTATION, orientation);
}

// ------------------------------------------------------------------------
// Retrieve the listener's gain adjustment (range = [0.0, inf), 
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundListenerAttribute::getGain()
{
    float gain;

    // Get the current listener's gain from OpenAL
    alGetListenerfv(AL_GAIN, &gain);

    // Return the gain
    return (double)gain;
}

// ------------------------------------------------------------------------
// Adjust the listener gain
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::setGain(double gain)
{
    // Set the listener's gain to the specified value
    alListenerf(AL_GAIN, (float)gain);
}
