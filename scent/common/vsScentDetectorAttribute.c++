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
//    VESS Module:  vsScentDetectorAttribute.c++
//
//    Description:  Attribute to maintain the location of the recipient of
//                  scent sources in the scene (usually tied to the user's
//                  viewpoint).
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsScentDetectorAttribute.h++"
#include "vsScentManager.h++"

// ------------------------------------------------------------------------
// Constructs a vsScentDetector object
// ------------------------------------------------------------------------
vsScentDetectorAttribute::vsScentDetectorAttribute()
{
    // Initialize instance variables
    parentComponent = NULL;
    offsetMatrix.setIdentity();
    currentPosition.clear();

    // Set the scent source parameters to defaults
    sensitivity = VS_SD_DEFAULT_SENSITIVITY;

    // Register with the scent manager
    vsScentManager::getInstance()->setScentDetector(this);
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsScentDetectorAttribute::~vsScentDetectorAttribute()
{
    // Unregister from the scent manager
    vsScentManager::getInstance()->removeScentDetector(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches the scent source to the specified node
// ------------------------------------------------------------------------
void vsScentDetectorAttribute::attach(vsNode *theNode)
{
    // Make sure the attribute isn't attached elsewhere
    if (attachedCount)
    {
        printf("vsScentDetectorAttribute::attach: Attribute is already "
            "attached\n");
        return;
    }

    // Only allow attachment to components
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsScentDetectorAttribute::attach: Can only attach scent "
            "detector attributes to vsComponents\n");
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
void vsScentDetectorAttribute::detach(vsNode *theNode)
{
    // Make sure the attribute is actually attached to the node
    if (!attachedCount)
    {
        printf("vsScentDetectorAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Detach from the node
    parentComponent = NULL;

    // Flag this attribute as not attached to a component
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// VESS internal function
// Intended to attach a duplicate of this attribute to the given node. This
// operation is not possible for this type of attribute because there can
// only be a single detector per application, and thus one container on the
// tree.
// ------------------------------------------------------------------------
void vsScentDetectorAttribute::attachDuplicate(vsNode *theNode)
{
}

// ------------------------------------------------------------------------
// Returns the current position of the scent source (as of the last
// call to update() )
// ------------------------------------------------------------------------
vsVector vsScentDetectorAttribute::getPosition()
{
    return currentPosition;
}

// ------------------------------------------------------------------------
// Causes the attribute to calculate the total transform to its parent
// node, and assign that data to the associated alDetector object
// ------------------------------------------------------------------------
void vsScentDetectorAttribute::update()
{
    vsMatrix       result;

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
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsScentDetectorAttribute::getClassName()
{
    return "vsScentDetectorAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsScentDetectorAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SCENT_DETECTOR;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsScentDetectorAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_OTHER;
}

// ------------------------------------------------------------------------
// Sets the offset matrix for this attribute. The offset matrix is
// multiplied into the overall transform matrix before it is used to
// set the source's global position.
// ------------------------------------------------------------------------
void vsScentDetectorAttribute::setOffsetMatrix(vsMatrix newMatrix)
{
    offsetMatrix = newMatrix;
}

// ------------------------------------------------------------------------
// Retrieves the offset matrix for this attribute
// ------------------------------------------------------------------------
vsMatrix vsScentDetectorAttribute::getOffsetMatrix()
{
    return offsetMatrix;
}

// ------------------------------------------------------------------------
// Return the current sensitivity factor
// ------------------------------------------------------------------------
double vsScentDetectorAttribute::getSensitivity()
{
    return sensitivity;
}

// ------------------------------------------------------------------------
// Set the detector sensitivity.  Valid range is from 0.0 to infinity
// ------------------------------------------------------------------------
void vsScentDetectorAttribute::setSensitivity(double newSensitivity)
{
    // Set the new scale, making sure the value is nonnegative
    if (newSensitivity < 0.0)
        sensitivity = 0.0;
    else
        sensitivity = newSensitivity;
}
