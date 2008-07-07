
#include "vsCOLLADAKeyframe.h++"

#include <string.h>

// ------------------------------------------------------------------------
// Creates a keyframe using the given data
// ------------------------------------------------------------------------
vsCOLLADAKeyframe::vsCOLLADAKeyframe(double t, int count, double *d)
{
    // Make sure the count is valid
    if (count < 0)
        count = 0;
    if (count > 16)
        count = 16;
    
    // Copy the data
    time = t;
    memset(data, 0, sizeof(data));
    memcpy(data, d, sizeof(double) * count);
}

// ------------------------------------------------------------------------
// Creates an empty keyframe
// ------------------------------------------------------------------------
vsCOLLADAKeyframe::vsCOLLADAKeyframe()
{
    time = 0.0;
    memset(data, 0, sizeof(data));
}

// ------------------------------------------------------------------------
// Destructor, does nothing
// ------------------------------------------------------------------------
vsCOLLADAKeyframe::~vsCOLLADAKeyframe()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCOLLADAKeyframe::getClassName()
{
    return "vsCOLLADAKeyframe";
}

// ------------------------------------------------------------------------
// Return the keyframe time
// ------------------------------------------------------------------------
double vsCOLLADAKeyframe::getTime()
{
    return time;
}

// ------------------------------------------------------------------------
// Sets a new keyframe time
// ------------------------------------------------------------------------
void vsCOLLADAKeyframe::setTime(double newTime)
{
    time = newTime;
}

// ------------------------------------------------------------------------
// Sets the keyframe data
// ------------------------------------------------------------------------
void vsCOLLADAKeyframe::setData(int count, double *values)
{
    // Make sure the count is valid
    if (count <= 0)
        return;
    if (count > 16)
        count = 16;

    // Copy the data
    memcpy(data, values, sizeof(double) * count);
}

// ------------------------------------------------------------------------
// Sets the keyframe data at the given index
// ------------------------------------------------------------------------
void vsCOLLADAKeyframe::setData(int index, double value)
{
    // Make sure the index is valid
    if ((index < 0) || (index >= 16))
        return;

    // Copy the data
    data[index] = value;
}

// ------------------------------------------------------------------------
// Return the keyframe data value at the given index
// ------------------------------------------------------------------------
double vsCOLLADAKeyframe::getData(int index)
{
    // Make sure the index is valid (else just return 0.0)
    if ((index >= 0) && (index < 16))
        return data[index];
    else
        return 0.0;
}

