#include <AL/al.h>
#include <sys/time.h>
#include <stdio.h>

#include "vsSoundListenerAttribute.h++"
#include "vsNode.h++"

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

    zero[0] = 0.0;
    zero[1] = 0.0;
    zero[2] = 0.0;
    zero[3] = 0.0;
    zero[4] = 0.0;
    zero[5] = 0.0;

    lastPos.clear();
    lastOrn.clear();

    // Call getTimeInterval to initialize the lastTime variable
    getTimeInterval();

    // Set up a coordinate conversion quaternion
    coordXform.setAxisAngleRotation(1, 0, 0, -90.0);
    coordXformInv = coordXform;
    coordXformInv.conjugate();

    // Configure the listener
    alListenerfv(AL_POSITION, zero);
    alListenerfv(AL_ORIENTATION, zero);
    alListenerfv(AL_VELOCITY, zero);
}

// ------------------------------------------------------------------------
// Destructor - Detatched this attribute from its associated view object
// ------------------------------------------------------------------------
vsSoundListenerAttribute::~vsSoundListenerAttribute()
{
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
    return VS_ATTRIBUTE_CATEGORY_OTHER;
}

// ------------------------------------------------------------------------
// Returns the interval of time since the last time this function was 
// called (based on the lastTime variable)
// ------------------------------------------------------------------------
double vsSoundListenerAttribute::getTimeInterval()
{
    struct timeval tv;
    double         currentTime;
    double         deltaTime;

    gettimeofday(&tv, NULL);

    currentTime = tv.tv_sec + tv.tv_usec / 1E6;
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    return deltaTime;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsSoundListenerAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    if (theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY)
    {
        printf("vsSoundListenerAttribute::attach: Can't attach sound source "
            "attributes to geometry nodes\n");
        return;
    }

    parentComponent = ((vsComponent *)theNode);
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::detach(vsNode *theNode)
{
    if (!attachedFlag)
    {
        printf("vsSoundListenerAttribute::detach: Attribute is not attached\n");
        return;
    }

    parentComponent = NULL;
    
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is sent to the 
// OpenAL sound source.
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::setOffsetMatrix(vsMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
vsMatrix vsSoundListenerAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to its associated alListener object.
// -----------------------------------------------------------------------
void vsSoundListenerAttribute::update()
{
    vsMatrix       result;
    vsQuat         tempQuat;
    vsVector       tempVec;
    vsVector       deltaVec;
    double         interval;
    vsVector       atVec, upVec;
    ALfloat        orientation[6];

    if (!attachedFlag)
        return;

    // Get the component's global transform
    result = parentComponent->getGlobalXform();

    result = result * offsetMatrix;
    
    // Update the position
    tempVec[VS_X] = result[0][3];
    tempVec[VS_Y] = result[1][3];
    tempVec[VS_Z] = result[2][3];
    tempVec = coordXform.rotatePoint(tempVec);

    // Update the OpenAL listener's position
    alListener3f(AL_POSITION, (float)tempVec[VS_X], (float)tempVec[VS_Y], 
        (float)tempVec[VS_Z]); 

    // Update the velocity (based on the last frame's position)
    deltaVec = tempVec - lastPos;
    interval = getTimeInterval();

    if (interval > 0.0)
    {
        deltaVec.scale(1/interval);
        alListener3f(AL_VELOCITY, (float)deltaVec[VS_X], (float)deltaVec[VS_Y], 
            (float)deltaVec[VS_Z]);

        lastPos = tempVec;
    }

    // Update the orientation
    tempQuat.setMatrixRotation(result);
    tempQuat = coordXform * tempQuat * coordXformInv;

    // Convert the vsQuat rotation to at/up vectors
    // In OpenAL (like OpenGL) -Z is forward (at) and +Y is up
    atVec.set(0.0, 0.0, -1.0);
    upVec.set(0.0, 1.0, 0.0);

    // Rotate the vectors by tempQuat
    atVec = tempQuat.rotatePoint(atVec);
    upVec = tempQuat.rotatePoint(upVec);

    // Update the alListener's orientation
    orientation[0] = (ALfloat)(atVec[VS_X]);
    orientation[1] = (ALfloat)(atVec[VS_Y]);
    orientation[2] = (ALfloat)(atVec[VS_Z]);
    orientation[3] = (ALfloat)(upVec[VS_X]);
    orientation[4] = (ALfloat)(upVec[VS_Y]);
    orientation[5] = (ALfloat)(upVec[VS_Z]);
    alListenerfv(AL_ORIENTATION, orientation);
}

// ------------------------------------------------------------------------
// Retrieve the listener's gain adjustment (range = [0.0, inf), 
// default = 1.0)
// ------------------------------------------------------------------------
double vsSoundListenerAttribute::getGain()
{
    float gain;

    alGetListenerf(AL_GAIN, &gain);

    return (double)gain;
}

// ------------------------------------------------------------------------
// Adjust the listener gain
// ------------------------------------------------------------------------
void vsSoundListenerAttribute::setGain(double gain)
{
    alListenerf(AL_GAIN, (float)gain);
}
