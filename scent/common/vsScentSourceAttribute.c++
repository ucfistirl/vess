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
//    VESS Module:  vsScentSourceAttribute.c++
//
//    Description:  Attribute to maintain the location of a source of odor
//                  in the VESS scene
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsScentSourceAttribute.h++"
#include "vsScentManager.h++"

// ------------------------------------------------------------------------
// Constructs a vsScentSourceAttribute using the given scent object
// ------------------------------------------------------------------------
vsScentSourceAttribute::vsScentSourceAttribute(vsScent *theScent)
{
    // Save the scent object
    scent = theScent;

    // Initialize class variables
    parentComponent = NULL;
    offsetMatrix.setIdentity();
    currentPosition.clear();

    // Set the scent source parameters to defaults
    scale = VS_SA_DEFAULT_SCALE;
    minStrength = VS_SA_DEFAULT_MIN_STRENGTH;
    maxStrength = VS_SA_DEFAULT_MAX_STRENGTH;
    refDistance = VS_SA_DEFAULT_REF_DIST;
    maxDistance = VS_SA_DEFAULT_MAX_DIST;
    rolloffFactor = VS_SA_DEFAULT_ROLLOFF;

    // Set the state variables (scent is on, occlusion is off)
    scentOn = true;
    occlusionOn = false;

    // Register scent source with the scent manager
    vsScentManager::getInstance()->addScentSource(this);
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsScentSourceAttribute::~vsScentSourceAttribute()
{
    // Unregister from the scent manager
    vsScentManager::getInstance()->removeScentSource(this);
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsScentSourceAttribute::getClassName()
{
    return "vsScentSourceAttribute";
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches the scent source to the specified node
// ------------------------------------------------------------------------
void vsScentSourceAttribute::attach(vsNode *theNode)
{
    // Make sure the attribute isn't attached elsewhere
    if (attachedCount)
    {
        printf("vsScentSourceAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    // Only allow attachment to components
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsScentSourceAttribute::attach: Can only attach scent source "
            "attributes to vsComponents\n");
        return;
    }

    // Attach to the given component
    parentComponent = ((vsComponent *)theNode);

    // Flag this attribute as attached to a component
    attachedCount = 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsScentSourceAttribute::detach(vsNode *theNode)
{
    // Make sure the attribute is actually attached to the node
    if (!attachedCount)
    {
        printf("vsScentSourceAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Detach from the node
    parentComponent = NULL;

    // Flag this attribute as not attached to a component
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsScentSourceAttribute::attachDuplicate(vsNode *theNode)
{
    vsScentSourceAttribute *source;

    // Create a duplicate attribute
    source = new vsScentSourceAttribute(scent);

    // Attach it to the given node
    theNode->addAttribute(source);
}

// ------------------------------------------------------------------------
// Returns the current position of the scent source (as of the last
// call to update() )
// ------------------------------------------------------------------------
atVector vsScentSourceAttribute::getPosition()
{
    return currentPosition;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to the associated alSource object
// ------------------------------------------------------------------------
void vsScentSourceAttribute::update()
{
    atMatrix       result;

    // If we're not attached to a component, we have nothing to do
    if (!attachedCount)
        return;

    // Get the global transform for this attribute's component and
    // apply the source's offset matrix.  This lets us know where in
    // global space the sound source is.
    result = parentComponent->getGlobalXform();
    result = result * offsetMatrix;

    // Update the current position
    currentPosition.clear();
    currentPosition = result.getPointXform(currentPosition);
}
// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsScentSourceAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SCENT_SOURCE;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsScentSourceAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_OTHER;
}

// ------------------------------------------------------------------------
// Turns the scent source on, allowing the olfactory device to emit odor.
// Scent sources are on by default.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::on()
{
    scentOn = true;
}

// ------------------------------------------------------------------------
// Turns the scent source off, preventing any odor from being emitted by
// the hardware.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::off()
{
    scentOn = false;
}

// ------------------------------------------------------------------------
// Returns whether or not the scent source is on
// ------------------------------------------------------------------------
bool vsScentSourceAttribute::isOn()
{
    return scentOn;
}

// ------------------------------------------------------------------------
// Enables occlusion testing, allowing this source to be occluded by 
// virtual objects and barriers.  Occlusion of sources is off by default.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::enableOcclusion()
{
    occlusionOn = true;
}

// ------------------------------------------------------------------------
// Disables occlusion testing, allowing the scent to be detected through
// virtual barriers (walls, etc).
// ------------------------------------------------------------------------
void vsScentSourceAttribute::disableOcclusion()
{
    occlusionOn = false;
}

// ------------------------------------------------------------------------
// Returns whether or not occlusion testing is enabled for this scent 
// source
// ------------------------------------------------------------------------
bool vsScentSourceAttribute::isOcclusionEnabled()
{
    return occlusionOn;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is used to
// set the source's global position.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setOffsetMatrix(atMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
atMatrix vsScentSourceAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Return the vsScent attached to this source
// ------------------------------------------------------------------------
vsScent *vsScentSourceAttribute::getScent()
{
    return scent;
}

// ------------------------------------------------------------------------
// Return the current scent strength scale factor for this source
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getStrengthScale()
{
    return scale;
}

// ------------------------------------------------------------------------
// Set the scent strength scale factor for this source.  Valid range is
// from 0.0 to 1.0, inclusive
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setStrengthScale(double newScale)
{
    // Set the new scale, clamping the value to [0.0,1.0]
    if (newScale < 0.0)
        scale = 0.0;
    else if (newScale > 1.0)
        scale = 1.0;
    else
        scale = newScale;
}

// ------------------------------------------------------------------------
// Return the current minimum strength of the scent
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getMinStrength()
{
    return minStrength;
}

// ------------------------------------------------------------------------
// Set the scent's minimum strength.  Valid range is from 0.0 to 1.0,
// inclusive
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setMinStrength(double newMin)
{
    // Set the new minimum, clamping the value to [0.0,1.0]
    if (newMin < 0.0)
        minStrength = 0.0;
    else if (newMin > 1.0)
        minStrength = 1.0;
    else
        minStrength = newMin;
}

// ------------------------------------------------------------------------
// Return the current maximum strength of the scent
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getMaxStrength()
{
    return maxStrength;
}

// ------------------------------------------------------------------------
// Set the scent's maximum strength.  Valid range is from 0.0 to 1.0,
// inclusive
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setMaxStrength(double newMax)
{
    // Set the new maximum, clamping the value to [0.0,1.0]
    if (newMax < 0.0)
        maxStrength = 0.0;
    else if (newMax > 1.0)
        maxStrength = 1.0;
    else
        maxStrength = newMax;
}

// ------------------------------------------------------------------------
// Return the current reference distance (the distance at which the scent
// is as strong as it can be)
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getReferenceDistance()
{
    return refDistance;
}

// ------------------------------------------------------------------------
// Set the reference distance (the distance at which the scent is as strong
// as it can be).  Valid values are from 0.0 (inclusive) to infinity
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setReferenceDistance(double distance)
{
    // Set the new reference distance, making sure it is not negative
    if (distance < 0.0)
        refDistance = 0.0;
    else
        refDistance = distance;
}

// ------------------------------------------------------------------------
// Return the maximum distance at which the scent can be detected
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getMaxDistance()
{
    return maxDistance;
}

// ------------------------------------------------------------------------
// Set the maximum distance at which the scent can be detected.  All values
// are valid, but a negative value indicates there is no maximum distance.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setMaxDistance(double distance)
{
    maxDistance = distance;
}

// ------------------------------------------------------------------------
// Return the current rolloff factor for this source.  The rolloff factor
// determines how quickly the scent dissipates with increasing distance
// ------------------------------------------------------------------------
double vsScentSourceAttribute::getRolloffFactor()
{
    return rolloffFactor;
}

// ------------------------------------------------------------------------
// Set the rolloff factor.  The rolloff factor determines how quickly the
// scent dissipates with distance.  Valid values are from 0.0 (inclusive)
// to infinity.
// ------------------------------------------------------------------------
void vsScentSourceAttribute::setRolloffFactor(double factor)
{
    // Set the new rolloff factor, making sure the value is non-negative
    if (factor < 0.0)
        rolloffFactor = 0.0;
    else
        rolloffFactor = factor;
}
