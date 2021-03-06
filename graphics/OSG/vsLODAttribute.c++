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
//    VESS Module:  vsLODAttribute.c++
//
//    Description:  Specifies that the children of the component are all
//                  levels-of-detail of the same object and are not to be
//                  drawn all at the same time; only one of the children
//                  should be drawn, with the determination of which to
//                  draw based on the distance from the viewer to the
//                  object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsLODAttribute.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsLODAttribute::vsLODAttribute()
{
    // Start unattached with no OSG LOD node
    osgLOD = NULL;
    attachedCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsLODAttribute::~vsLODAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsLODAttribute::getClassName()
{
    return "vsLODAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsLODAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_LOD;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsLODAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_GROUPING;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsLODAttribute::clone()
{
    vsLODAttribute *newAttrib;

    // We can't really clone an LOD without a node to attach it to (the
    // number of LOD ranges depends on the number of children on the
    // node), so just create a new LOD attribute and return it
    newAttrib = new vsLODAttribute();
    return newAttrib;
}

// ------------------------------------------------------------------------
// Sets a user-defined center for this LOD.  The default center is
// the center of the parent node's bounding sphere.
// ------------------------------------------------------------------------
void vsLODAttribute::setCenter(atVector newCenter)
{
    osg::Vec3 osgVec;

    // If we're not yet attached, print a warning and bail out
    if (!attachedCount)
    {
        printf("vsLODAttribute::setRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return;
    }

    // Set the new center on the OSG LOD node
    osgVec.set(newCenter[AT_X], newCenter[AT_Y], newCenter[AT_Z]);
    osgLOD->setCenter(osgVec);
}

// ------------------------------------------------------------------------
// Returns the current center of this LOD.
// ------------------------------------------------------------------------
atVector vsLODAttribute::getCenter()
{
    osg::Vec3 osgVec;

    // If we're not yet attached, print a warning and bail out
    if (!attachedCount)
    {
        printf("vsLODAttribute::setRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return atVector(0,0,0);
    }

    // Get the current center from the LOD node
    osgVec = osgLOD->getCenter();

    // Return it in atVector form
    return atVector(osgVec[0], osgVec[1], osgVec[2]);
}

// ------------------------------------------------------------------------
// Sets the far limit for which the child with index childNum on the
// parent component is displayed. The near limit is the far limit of the
// child with the next lower index, or 0 for the child at index 0. The
// first child has an index of 0.
// ------------------------------------------------------------------------
void vsLODAttribute::setRangeEnd(int childNum, double rangeLimit)
{
    double minRange, maxRange;

    // If we're not yet attached, print a warning and bail out
    if (!attachedCount)
    {
        printf("vsLODAttribute::setRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return;
    }

    // Make sure the child index is valid
    if ((childNum < 0) || (childNum >= (int)osgLOD->getNumChildren()))
    {
        printf("vsLODAttribute::setRangeEnd: Index out of bounds\n");
        return;
    }

    // See if we're configuring the first child or not
    if (childNum == 0)
    {
        // We're configuring the first child, use 0.0 for the minimum
        // range
        osgLOD->setRange(childNum, 0.0, rangeLimit);
    }
    else
    {
        // We're not configuring the first child, use the sibling to the
        // left to get the minimum range (left sibling's max range is this
        // LOD's min range).
        minRange = osgLOD->getMaxRange(childNum - 1);
        osgLOD->setRange(childNum, minRange, rangeLimit);
    }

    // If there is a sibling to the right of this one, also set the specified
    // range limit as the minimum range for the next LOD
    if ((int)osgLOD->getNumRanges() > (childNum+1))
    {
        maxRange = osgLOD->getMaxRange(childNum+1);
        osgLOD->setRange(childNum+1, rangeLimit, maxRange);
    }
}

// ------------------------------------------------------------------------
// Retrieves the far distance limit for which this child is displayed.
// The index of the first child is 0.
// ------------------------------------------------------------------------
double vsLODAttribute::getRangeEnd(int childNum)
{
    // If we're not yet attached, print a warning and bail out
    if (!attachedCount)
    {
        printf("vsLODAttribute::getRangeEnd: Attribute must be attached "
            "before LOD can be manipulated\n");
        return 0.0;
    }

    // Make sure the child index is valid
    if ((childNum < 0) || (childNum >= (int)osgLOD->getNumChildren()))
    {
        printf("vsLODAttribute::getRangeEnd: Index out of bounds\n");
        return 0.0;
    }

    // Return the maximum range for the given child
    return osgLOD->getMaxRange(childNum);
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsLODAttribute::canAttach()
{
    // Only one node attachment allowed, return false if we're attached
    if (attachedCount)
        return false;

    // Otherwise, return true
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::attach(vsNode *theNode)
{
    int loop, childCount;

    // If we're already attached, bail out
    if (attachedCount)
    {
        printf("vsLODAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Make sure the attaching node is a component (no other node makes
    // sense for an LOD attribute
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsLODAttribute::attach: Can only attach LOD "
            "attributes to vsComponents\n");
        return;
    }
    
    // Replace the bottom group with an LOD group
    osgLOD = new osg::LOD();
    ((vsComponent *)theNode)->replaceBottomGroup(osgLOD);
    
    // Set the LOD ranges to reasonable defaults
    childCount = osgLOD->getNumChildren();
    for (loop = 0; loop < childCount; loop++)
        osgLOD->setRange(loop, 
            (1000.0 * (double)loop / (double)childCount),
            (1000.0 * (double)(loop+1) / (double)childCount));

    // Set the attached flag to true
    attachedCount = true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLODAttribute::detach(vsNode *theNode)
{
    osg::Group *newGroup;

    // If we're not attached to a node, we don't need to detach
    if (!attachedCount)
    {
        printf("vsLODAttribute::detach: Attribute is not attached\n");
        return;
    }
    
    // Replace the LOD group with an ordinary group
    newGroup = new osg::Group();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    osgLOD = NULL;
    
    // Clear the attached flag
    attachedCount = false;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLODAttribute::attachDuplicate(vsNode *theNode)
{
    vsLODAttribute *newAttrib;
    int loop;
    vsComponent *theComponent;
    
    // Make sure the node we're given is a component (no other node
    // makes sense for an LOD attribute)
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;

    // Create a new LOD attribute
    newAttrib = new vsLODAttribute();

    // Add the attribute to the given node
    theNode->addAttribute(newAttrib);

    // Copy the LOD ranges from this attribute to the new one
    for (loop = 0; loop < theComponent->getChildCount(); loop++)
        newAttrib->setRangeEnd(loop, getRangeEnd(loop));
}

// ------------------------------------------------------------------------
// Internal function
// Recomputes the lower-bounds for each child, in case a child was added
// or removed without us knowing about it
// ------------------------------------------------------------------------
void vsLODAttribute::apply()
{
    int loop;
    float nearDist, farDist;
    
    // Iterate over all LOD ranges on the osg::LOD node
    for (loop = 0; loop < (int)osgLOD->getNumRanges(); loop++)
    {
        // Get the maximum range on the previous child (use 0.0 if this
        // is the first child)
        if (loop == 0)
            nearDist = 0.0;
        else
            nearDist = osgLOD->getMaxRange(loop - 1);

        // Get the maximum range on this child
        farDist = osgLOD->getMaxRange(loop);
        
        // Set the near and far ranges on this child to the values we've
        // found
        osgLOD->setRange(loop, nearDist, farDist);
        
        // Print a warning to the user if the ranges don't make sense
        if (nearDist > farDist)
            printf("vsLODAttribute::apply: Minimum range > maximum range "
                "for child %d\n", loop);
    }
}
