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
//    VESS Module:  vsCgParameterBlockAttribute.c++
//
//    Description:  Attribute to contain CgParameters that are applied to
//                  the scene from where this attribute is attached down.
//                  It correlates to the osgNV::ParameterBlock.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsCgParameterBlockAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Initializes shading to Gouraud
// ------------------------------------------------------------------------
vsCgParameterBlockAttribute::vsCgParameterBlockAttribute()
{
    // Create a new osgNV::ParameterBlock attribute and reference it
    parameterBlock = new osgNV::ParameterBlock();
    parameterBlock->ref();

    // Create the array that will store a reference to parameters added
    parameterArray = new vsGrowableArray(10, 5);
    parameterCount = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgParameterBlockAttribute::~vsCgParameterBlockAttribute()
{
    vsCgParameter    *tempParameter;

    // Unreference the OSGNV ParameterBlock.
    parameterBlock->unref();

    // Unreference delete on all stored parameters.
    for (parameterCount--; parameterCount >= 0; parameterCount--)
    {
        tempParameter =
            (vsCgParameter *) parameterArray->getData(parameterCount);
        vsObject::unrefDelete(tempParameter);
    }

    delete parameterArray;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCgParameterBlockAttribute::getClassName()
{
    return "vsCgParameterBlockAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsCgParameterBlockAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_CG_PARAMETER_BLOCK;
}

// ------------------------------------------------------------------------
// Add a parameter to the parameter block
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::addCgParameter(vsCgParameter *parameter)
{
    // Reference the given paramter.
    parameter->ref();

    // Add its corresponding osgNV representation to the osgNV parameterBlock.
    parameterBlock->addParameter(parameter->getCgParameter());

    // Add it to our array to keep track of it, and increment the count.
    parameterArray->setData(parameterCount, parameter);
    parameterCount++;
}

// ------------------------------------------------------------------------
// Remove a parameter from the parameter block
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::removeCgParameter(vsCgParameter *parameter)
{
    vsCgParameter    *tempParameter;
    int index;
    bool found;

    found = false;
    
    // Search for the given parameter.
    for (index = 0; (index < parameterCount) && !found; index++)
    {
        if (parameterArray->getData(index) == parameter)
            found = true;
    }

    // If it was found, remove it and slide down all parameters after to
    // take its palce.
    if (found)
    {
        // Remove the corresponding parameter entry in the osgNV ParameterBlock.
        parameterBlock->removeParameter(index);

        // Unref the parameter we are removing.
        tempParameter = (vsCgParameter *) parameterArray->getData(index);
        tempParameter->unref();
// Should I unrefDelete?
        //vsObject::unrefDelete(tempParameter);

        parameterCount--;

        // Slide down the rest of the parameters.
        for (; index < parameterCount; index++)
        {
            parameterArray->setData(index, parameterArray->getData(index+1));
        }

        // Set the last entry to NULL.
        parameterArray->setData(parameterCount, NULL);
    }
}

// ------------------------------------------------------------------------
// Return the parameter at the given index, if it exists.
// ------------------------------------------------------------------------
vsCgParameter *vsCgParameterBlockAttribute::getCgParameter(int index)
{
    if ((index >= 0) && (index < parameterCount))
        return (vsCgParameter *) parameterArray->getData(index);
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Return the number of parameters in this block.
// ------------------------------------------------------------------------
int vsCgParameterBlockAttribute::getCgParameterCount()
{
    return parameterCount;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;

    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsCgParameterBlockAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet on the given node
    osgStateSet = getOSGStateSet(node);

    // Apply the osgNV::ParameterBlock on the StateSet
    osgStateSet->setAttributeAndModes(parameterBlock, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::attach(vsNode *node)
{
    // Do standard vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Set up the osg::StateSet on this node to use the osgNV::ParameterBlock
    // we've created
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the node's StateSet and reset the ParameterBlock mode to inherit
    osgStateSet = getOSGStateSet(node);
    osgStateSet->setAttributeAndModes(parameterBlock,
        osg::StateAttribute::INHERIT);

    // Do standard vsStateAttribute detaching
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsCgParameterBlockAttribute::attachDuplicate(vsNode *theNode)
{
    vsCgParameterBlockAttribute *newAttrib;
    int index;
    
    // Create a new vsCgParameterBlockAttribute and copy the parameters from
    // this attribute to the new one
    newAttrib = new vsCgParameterBlockAttribute();
    for (index = 0; index < parameterCount; index++)
    {
        newAttrib->addCgParameter(getCgParameter(index));
    }

    // Add the new attribute to the given node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsCgParameterBlockAttribute::isEquivalent(vsAttribute *attribute)
{
    vsCgParameterBlockAttribute *attr;
    int index;
    
    // Make sure the given attribute is valid, return FALSE if not
    if (!attribute)
        return false;

    // Check if we're comparing the attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a shading attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_CG_PARAMETER_BLOCK)
        return false;

    // Cast the given attribute to a shading attribute
    attr = (vsCgParameterBlockAttribute *)attribute;

    // Compare the parameter pointers
    for (index = 0; index < parameterCount; index++)
    {
        if (attr->getCgParameter(index) != getCgParameter(index))
            return false;
    }

    // Return true if we get this far (they are equivalent)
    return true;
}
