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
//    VESS Module:  vsDecalAttribute.c++
//
//    Description:  OSG-specific class that implements a callback which
//                  is called when an OSG cull traversal reaches a
//                  vsComponent with a vsDecalAttribute attached
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsDecalCallback.h++"
#include "vsDecalAttribute.h++"
#include "vsOSGStateSet.h++"
#include <osg/PolygonOffset>
#include <osgUtil/CullVisitor>

//------------------------------------------------------------------------
// Constructor
// Stores the pointer to the parent decal attribute
//------------------------------------------------------------------------
vsDecalCallback::vsDecalCallback(vsDecalAttribute *decalAttrib)
{
    // Store the decal attribute we're controlling
    decalAttr = decalAttrib;

    // Create an atArray to hold the StateSet objects
    stateSetArray = new atArray();
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsDecalCallback::~vsDecalCallback()
{
    // Delete the state set array (this will unreference and potentially
    // delete all of the state sets in it)
    delete stateSetArray;
}

//------------------------------------------------------------------------
// OSG callback function
// Called when a cull traversal reaches a vsComponent with a decal
// attribute attached. Handles the cull traversal of the node's children
// by hand, manipulating the state stack to include a state set containing
// a polygon offset attribute with the proper offset values for that
// child.
//------------------------------------------------------------------------
void vsDecalCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    osg::Group *decalGroup;
    int loop;
    osg::Node *childNode;
    osgUtil::CullVisitor *cullVisitor;
    vsOSGStateSet *wrapper;
    osg::StateSet *osgStateSet;
    
    // Make sure we have a Group
    decalGroup = dynamic_cast<osg::Group *>(node);
    if (!decalGroup)
        return;

    // Make sure the NodeVisitor is a CullVisitor
    cullVisitor = dynamic_cast<osgUtil::CullVisitor *>(nv);
    if (!cullVisitor)
        return;

    // Make sure we have enough state sets to go around
    checkSize(decalGroup->getNumChildren());
    if (stateSetArray->getNumEntries() < (int)decalGroup->getNumChildren())
        return;

    // Run the cull traversal, by hand, on each of the Group's children
    for (loop = 0; loop < (int)decalGroup->getNumChildren(); loop++)
    {
        // Push the polygon offset state set onto the CullVisitor's stack
        wrapper = (vsOSGStateSet *) stateSetArray->getEntry(loop);
        osgStateSet = wrapper->getStateSet();
        cullVisitor->pushStateSet(osgStateSet);

        // Traverse the child
        childNode = decalGroup->getChild(loop);
        childNode->accept(*nv);
        
        // Pop the polygon offset state set off of the CullVisitor's stack
        cullVisitor->popStateSet();
    }
}

//------------------------------------------------------------------------
// Private function
// Verifies that there are at least as many state sets in our array as
// the specified size; creates more state sets and adds them if the array
// is currently too small.
//------------------------------------------------------------------------
void vsDecalCallback::checkSize(int newSize)
{
    osg::StateSet *osgStateSet;
    vsOSGStateSet *wrapper;
    osg::PolygonOffset *osgPolyOffset;
    
    if (newSize > 256)
    {
        printf("vsDecalCallback::checkSize: Bad array size\n");
        return;
    }
    
    // If we're short any state sets, create new StateSets with PolyOffsets
    // on them and add them to the state set array
    while (stateSetArray->getNumEntries() < newSize)
    {
        // New, empty StateSet
        osgStateSet = new osg::StateSet();

        // Store the state set in a VESS wrapper (this also maintains a
        // reference count on the state set for us)
        wrapper = new vsOSGStateSet(osgStateSet);

        // Clear the state set of any state attributes
        osgStateSet->clear();

        // New PolygonOffset with offset multiplier equal to array position
        osgPolyOffset = new osg::PolygonOffset();
        osgPolyOffset->setFactor(-1.0 * stateSetArray->getNumEntries());
        osgPolyOffset->setUnits(-20.0 * stateSetArray->getNumEntries());

        // Add the poly offset to the state set
        osgStateSet->setAttributeAndModes(osgPolyOffset);

        // Add the wrapped state set to our array
        stateSetArray->addEntry(wrapper);
    }
}
