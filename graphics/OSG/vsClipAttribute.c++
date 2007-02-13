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
//    VESS Module:  vsClipAttribute.c++
//
//    Description:  Applies one or more user-defined clipping planes to
//                  the subgraph of the scene where it is attached
//
//    Author(s):    Michael Prestia, Jason Daly
//
//------------------------------------------------------------------------
#include "vsClipAttribute.h++"

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vsClipAttribute::vsClipAttribute()
               : attachedNodes(5, 5)
{
    // Initialize the number of planes to 0
    numPlanes = 0;

    // Initialize the array of OSG ClipPlane's to NULL, so no planes are
    // active at first
    memset(planeArray, 0, sizeof(planeArray));
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsClipAttribute::~vsClipAttribute()
{
    // Iterate over the array of planes
    for (int i = 0; i < VS_CLIPATTR_MAX_PLANES; i++)
    {
        // If this clipping plane exists, remove it
        if (planeArray[i] != NULL)
            planeArray[i]->unref();
    }
}

// ------------------------------------------------------------------------
// Returns the string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsClipAttribute::getClassName()
{
    return "vsClipAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsClipAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_CLIP;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsClipAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_STATE;
}

// ------------------------------------------------------------------------
// Activates the current ClipPlane's on the OSG node's state set. This
// effectively applies the clipping planes to the scene
// ------------------------------------------------------------------------
void vsClipAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    int i;

    // Start with the osg::StateAttribute modes set to ON
    attrMode = osg::StateAttribute::ON;

    // If this attribute's override flag is set, change the state attribute
    // mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the node's StateSet
    osgStateSet = getOSGStateSet(node);

    // Apply each osg::ClipPlane in our array
    for (i = 0; i < VS_CLIPATTR_MAX_PLANES; i++)
    {
        // See if this plane exists
        if (planeArray[i] != NULL)
        {
            // Apply the plane to the StateSet using the mode we calculated
            // above
            osgStateSet->setAttributeAndModes(planeArray[i], attrMode);
        }
    }
}

// ------------------------------------------------------------------------
// Sets the given clip plane to the given plane equation.  The parameters
// are coefficients for the plane equation Ax + By + Cz + D = 0
// ------------------------------------------------------------------------
void vsClipAttribute::setClipPlane(int planeIndex, double a, double b,
                                   double c, double d)
{
    osg::StateSet *stateset;
    osg::ClipPlane *plane;
    int i;

    // Validate the plane index (it can't be negative or greater than
    // the maximum)
    if ((planeIndex < 0) || (planeIndex >= VS_CLIPATTR_MAX_PLANES))
    {
        printf("vsClipAttribute::setClipPlane: Clip Plane index %d is "
           "invalid\n", planeIndex);
        return;
    }

    // See if we already have a plane at this index
    if (planeArray[planeIndex] != NULL)
    {
        // Update the plane with the new coefficients
        planeArray[planeIndex]->setClipPlane(a, b, c, d);
    }
    else
    {
        // Otherwise, create a new ClipPlane with the given coefficients
        plane = new osg::ClipPlane(planeIndex, a, b, c, d);

        // Add the new ClipPlane to our array at the specified index
        planeArray[planeIndex] = plane;
        planeArray[planeIndex]->ref();

        // Update the StateSets of any nodes we're attached to
        if (attachedCount > 0)
        {
            // Iterate over the nodes we're attached to
            for (i = 0; i < attachedCount; i++)
            {
                // Update the attribute and modes for this node
                setOSGAttrModes((vsNode *)(attachedNodes[i]));
            }
        }
    }
}

// ------------------------------------------------------------------------
// Sets the given clip plane to the plane formed by the given point and
// normal.
// ------------------------------------------------------------------------
void vsClipAttribute::setClipPlane(int planeIndex, vsVector pointOnPlane,
                                   vsVector normal)
{
    double d;
    double a, b, c;

    // Compute the plane equation coefficients from the given point and
    // normal
    a = normal[VS_X];
    b = normal[VS_Y];
    c = normal[VS_Z];
    d = -(normal.getDotProduct(pointOnPlane));

    // Construct the plane using the coefficients
    setClipPlane(planeIndex, a, b, c, d);
}

// ------------------------------------------------------------------------
// Removes a clip plane from a designated index of the array
// ------------------------------------------------------------------------
void vsClipAttribute::removeClipPlane(int planeIndex)
{
    int i;

    // Check the index
    if ((planeIndex < 0) || (planeIndex >= VS_CLIPATTR_MAX_PLANES))
    {
        printf("vsClipAttribute::removeClipPlane: Clip Plane index %d is "
           "invalid\n", planeIndex);
        return;
    }

    // If there's a ClipPlane at the given index, remove it
    if (planeArray[planeIndex] != NULL)
    {
        // Unref the plane and set the array entry to NULL
        planeArray[planeIndex]->unref();
        planeArray[planeIndex] = NULL;

        // Update the StateSets of any nodes we're attached to
        if (attachedCount > 0)
        {
            // Iterate over the nodes we're attached to
            for (i = 0; i < attachedCount; i++)
            {
                // Update the attribute and modes for this node
                setOSGAttrModes((vsNode *)(attachedNodes[i]));
            }
        }
    }
}

// ------------------------------------------------------------------------
// Returns whether or not the ClipPlane at the given index is active
// ------------------------------------------------------------------------
bool vsClipAttribute::isClipPlaneActive(int planeIndex)
{
    // Check the index
    if ((planeIndex < 0) || (planeIndex >= VS_CLIPATTR_MAX_PLANES))
        return false;

    // Return true if there is a ClipPlane at the given index, or false
    // if not
    if (planeArray[planeIndex] == NULL)
        return false;
    else
        return true;
}

// ------------------------------------------------------------------------
// Retrieve the plane of a certain index as a 4-dimensional vsVector
// ------------------------------------------------------------------------
void vsClipAttribute::getClipPlaneCoeffs(int planeIndex, double *a,
                                         double *b, double *c, double *d)
{
    osg::Vec4d vec;

    // Check the plane index for validity
    if ((planeIndex < 0) || (planeIndex >= VS_CLIPATTR_MAX_PLANES))
    {
        printf("vsClipAttribute::getClipPlaneCoeffs: Clip Plane index %d is "
           "invalid\n", planeIndex);
        return;
    }

    // If there is no plane at the given index, return all zeroes
    if (planeArray[planeIndex] == NULL)
    {
        // Return a zero for each coefficient, if the pointer in question
        // is not NULL
        if (a != NULL)
            *a = 0.0;
        if (b != NULL)
            *b = 0.0;
        if (c != NULL)
            *c = 0.0;
        if (d != NULL)
            *d = 0.0;
    }
    else
    {
        // Get the plane coefficients as an OSG Vec4d
        vec = planeArray[planeIndex]->getClipPlane();

        // Return the coefficients if there is space provided for them
        if (a != NULL)
            *a = vec.x();
        if (b != NULL)
            *b = vec.y();
        if (c != NULL)
            *c = vec.z();
        if (d != NULL)
            *d = vec.w();
    }
}

// ------------------------------------------------------------------------
// Returns the normal of the requested clipping plane, which is really
// just a normalized vector of the A, B, C coefficients of the plane
// equation
// ------------------------------------------------------------------------
vsVector vsClipAttribute::getClipPlaneNormal(int planeIndex)
{
    osg::Vec4d vec;

    // Check the plane index for validity
    if ((planeIndex < 0) || (planeIndex >= VS_CLIPATTR_MAX_PLANES))
    {
        printf("vsClipAttribute::getClipPlaneCoeffs: Clip Plane index %d is "
           "invalid\n", planeIndex);
        return vsVector(0.0, 0.0, 0.0);
    }

    // If there is no plane at the given index, return all zeroes
    if (planeArray[planeIndex] == NULL)
    {
        // Return a zero vector for the normal
        return vsVector(0.0, 0.0, 0.0);
    }
    else
    {
        // Get the plane coefficients as an OSG Vec4d
        vec = planeArray[planeIndex]->getClipPlane();

        // Return the plane's normal, as determined by the A, B, and C
        // coefficients of the clip plane in question
        return vsVector(vec.x(), vec.y(), vec.z()).getNormalized();
    }
}

// ------------------------------------------------------------------------
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsClipAttribute::isEquivalent(vsAttribute *attribute)
{
    vsClipAttribute *attr;
    int i;
    double a1, b1, c1, d1;
    double a2, b2, c2, d2;

    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return true;

    // Make sure the given attribute is a clip attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_CLIP)
        return false;

    // Cast the given attribute to a clip attribute
    attr = (vsClipAttribute *)attribute;

    // Compare the clip planes themselves
    for (i = 0; i < VS_CLIPATTR_MAX_PLANES; i++)
    {
        // First, see if the planes at index i are both active or inactive
        if (isClipPlaneActive(i) != attr->isClipPlaneActive(i))
            return false;

        // If the planes are active, compare their equations
        if (isClipPlaneActive(i))
        {
            // Get the plane coefficients
            getClipPlaneCoeffs(i, &a1, &b1, &c1, &d1);
            attr->getClipPlaneCoeffs(i, &a2, &b2, &c2, &d2);

            // Compare the coefficients, using a tolerance of 0.000001
            if (fabs(a2 - a1) > 1.0E-6)
                return false;
            if (fabs(b2 - b1) > 1.0E-6)
                return false;
            if (fabs(c2 - c1) > 1.0E-6)
                return false;
            if (fabs(d2 - d1) > 1.0E-6)
                return false;
        }
    }

    // If we get this far, the attributes are equivalent
    return true;
}

// ------------------------------------------------------------------------
// Notifies the atrtibute that it is being added to a given node's
// attribute list
// ------------------------------------------------------------------------
void vsClipAttribute::attach(vsNode *node)
{
    // Remember the node we're attaching to.  We need to be able to update
    // each attached node's StateSet if any of the planes change.
    attachedNodes[attachedCount] = node;
    
    // Do standard vsStateAttribute attaching (this includes incrementing
    // the attachedCount, so we don't do that ourselves)
    vsStateAttribute::attach(node);

    // Set the OSG modes this attribute is in charge of on the node
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsClipAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;
    int i;
    int attachedIndex;

    // Get the OSG StateSet from this node
    osgStateSet = getOSGStateSet(node);

    // Setting the modes to INHERIT removes these attributes from
    // the StateSet entirely
    for (i = 0; i < VS_CLIPATTR_MAX_PLANES; i++)
    {
        // If this plane exists, set it's mode to inherit
        if (planeArray[i] != NULL)
           osgStateSet->setAttributeAndModes(planeArray[i],
              osg::StateAttribute::INHERIT);
    }

    // Locate the node in our list of attached nodes
    attachedIndex = 0;
    while ((attachedIndex < attachedCount) &&
           ((vsNode *)(attachedNodes[attachedIndex]) != node))
        attachedIndex++;

    // If we found the node, remove it from the list
    if (attachedIndex < attachedCount)
    {
        // Remove the attached node from our list
        attachedNodes[attachedIndex] = NULL;

        // If the attached node isn't at the end of the list, slide the rest
        // into its place
        if (attachedIndex < (attachedCount-1))
        {
            memmove(&attachedNodes[attachedIndex],
                &attachedNodes[attachedIndex+1],
                (attachedCount-attachedIndex-1) * sizeof(vsNode *));
            attachedNodes[attachedCount-1] = NULL;
        }
    }

    // Detach from the node (this properly decrements attachedCount, so
    // we don't explicitly do it here)
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsClipAttribute::attachDuplicate(vsNode *theNode)
{
    vsClipAttribute *newAttrib;
    int i;
    osg::Vec4d planeCoeffs;
    
    // Create a new clip attribute
    newAttrib = new vsClipAttribute();

    // Copy the attribute information into the
    // new attribute
    for (i = 0; i < VS_CLIPATTR_MAX_PLANES; i++)
    {
        // See if this clipping plane is active
        if (planeArray[i] != NULL)
        {
            // Get the coefficients of this plane's equation
            // (Ax + By + Cz + D = 0)
            planeCoeffs = planeArray[i]->getClipPlane();

            // Set the corresponding plane on the new attribute
            newAttrib->setClipPlane(i, planeCoeffs.x(), planeCoeffs.y(),
                planeCoeffs.z(), planeCoeffs.w());
        }
    }

    // Attach it.
    theNode->addAttribute(newAttrib);
}
