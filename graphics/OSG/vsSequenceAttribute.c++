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
//    VESS Module:  vsSequenceAttribute.c++
//
//    Description:  Attribute that specifies that the children of the
//                  component are multiple frames of an animation
//                  sequence and should be drawn sequentially for the
//                  specified periods of time rather than all at once.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsSequenceAttribute.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsSequenceAttribute::vsSequenceAttribute()
{
    // Start with a NULL osg::Sequence
    osgSequence = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSequenceAttribute::~vsSequenceAttribute()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSequenceAttribute::getClassName()
{
    return "vsSequenceAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsSequenceAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_SEQUENCE;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsSequenceAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_GROUPING;
}

// ------------------------------------------------------------------------
// Sets the time that the child node with index childNum should be
// displayed for in the sequence. The number of the first child is 0.
// ------------------------------------------------------------------------
void vsSequenceAttribute::setChildTime(int childNum, double seconds)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return;
    }

    // Make sure the child number is valid
    if ((childNum < 0) || (childNum >= osgSequence->getNumChildren()))
    {
        printf("vsSequenceAttribute::setChildTime: Index out of bounds\n");
        return;
    }

    // Set the given frame time on the given child
    osgSequence->setTime(childNum, (float) seconds);
}

// ------------------------------------------------------------------------
// Retrieves the time that the child node with index childNum should be
// displayed for in the sequence. The number of the first child is 0.
// ------------------------------------------------------------------------
double vsSequenceAttribute::getChildTime(int childNum)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return 0.0;
    }

    // Make sure the child number is valid
    if ((childNum < 0) || (childNum >= osgSequence->getNumChildren()))
    {
        printf("vsSequenceAttribute::getChildTime: Index out of bounds\n");
        return 0.0;
    }

    // Fetch and return the given child's frame time
    return (double (osgSequence->getTime(childNum)));
}

// ------------------------------------------------------------------------
// Sets the number of times that this sequence should repeat itself. Note
// that for 'swing' cycle mode, each pass across the sequence counts as
// one repetition; going from start to end and back again counts as two.
// ------------------------------------------------------------------------
void vsSequenceAttribute::setRepetitionCount(int numReps)
{
    float speed;
    int temp;

    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // Get the current duration settings (so we know the speed value)
    osgSequence->getDuration(speed, temp);

    // Set the duration values back, with the new repetition count
    osgSequence->setDuration(speed, numReps);
}

// ------------------------------------------------------------------------
// Retrieves the number of times that this sequence should repeat itself
// ------------------------------------------------------------------------
int vsSequenceAttribute::getRepetitionCount()
{
    int result;
    float temp;

    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Fetch the current repetition count from the osg::Sequence
    osgSequence->getDuration(temp, result);

    // Return the repetition count
    return result;
}

// ------------------------------------------------------------------------
// Sets the cycle mode for this sequence
// ------------------------------------------------------------------------
void vsSequenceAttribute::setCycleMode(int seqCycle)
{
    osg::Sequence::LoopMode mode;
    int begin, end;

    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // Get the current sequence interval settings from OSG
    osgSequence->getInterval(mode, begin, end);
    
    // Translate the given cycle mode to its OSG counterpart and set
    // the new mode on the osg::Sequence
    switch (seqCycle)
    {
        case VS_SEQUENCE_CYCLE_FORWARD:
            osgSequence->setInterval(osg::Sequence::LOOP, begin, end);
            break;
        case VS_SEQUENCE_CYCLE_SWING:
            osgSequence->setInterval(osg::Sequence::SWING, begin, end);
            break;
        default:
            printf("vsSequenceAttribute::setCycleMode: Unrecognized cycle "
                "constant\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the cycle mode for this sequence
// ------------------------------------------------------------------------
int vsSequenceAttribute::getCycleMode()
{
    osg::Sequence::LoopMode mode;
    int begin, end;

    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Get the current interval settings from the osg::Sequence
    osgSequence->getInterval(mode, begin, end);
    
    // Translate the cycle mode to its VESS counterpart
    if (mode == osg::Sequence::SWING)
        return VS_SEQUENCE_CYCLE_SWING;
    else
        return VS_SEQUENCE_CYCLE_FORWARD;
}

// ------------------------------------------------------------------------
// Starts or stops the sequence playing
// ------------------------------------------------------------------------
void vsSequenceAttribute::setPlayMode(int playMode)
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // Translate the requested play mode to its OSG counterpart and
    // set the osg::Sequence's mode
    switch (playMode)
    {
        case VS_SEQUENCE_MODE_START:
            osgSequence->setMode(osg::Sequence::START);
            break;
        case VS_SEQUENCE_MODE_STOP:
            osgSequence->setMode(osg::Sequence::STOP);
            break;
        case VS_SEQUENCE_MODE_PAUSE:
            osgSequence->setMode(osg::Sequence::PAUSE);
            break;
        case VS_SEQUENCE_MODE_RESUME:
            osgSequence->setMode(osg::Sequence::RESUME);
            break;
        default:
            printf("vsSequenceAttribute::setPlayMode: Unrecognized mode "
                "constant\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Returns the current playing mode of the sequence
// ------------------------------------------------------------------------
int vsSequenceAttribute::getPlayMode()
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Fetch the play mode from the osg::Sequence and translate it to
    // its VESS counterpart
    switch (osgSequence->getMode())
    {
        case osg::Sequence::START:
            return VS_SEQUENCE_MODE_START;
        case osg::Sequence::STOP:
            return VS_SEQUENCE_MODE_STOP;
        case osg::Sequence::PAUSE:
            return VS_SEQUENCE_MODE_PAUSE;
        case osg::Sequence::RESUME:
            return VS_SEQUENCE_MODE_RESUME;
    }
    
    // Return 0 if we don't recognize the osg::Sequence's play mode
    return 0;
}

// ------------------------------------------------------------------------
// Returns the index of the current child being drawn. The index of the
// first child is 0.
// ------------------------------------------------------------------------
int vsSequenceAttribute::getCurrentChildNum()
{
    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCurrentChildNum: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Return the current switch value of the osg::Sequence
    return (osgSequence->getValue());
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsSequenceAttribute::canAttach()
{
    // If a node is already attached, we can't attach another one
    if (attachedFlag)
        return VS_FALSE;

    // Otherwise, return TRUE
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSequenceAttribute::attach(vsNode *theNode)
{
    int childCount, loop;

    // Make sure we're not attached to a node, bail out if we are
    if (attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Make sure the attaching node is a component (no other node can
    // receive this attribute)
    if (theNode->getNodeType() != VS_NODE_TYPE_COMPONENT)
    {
        printf("vsSequenceAttribute::attach: Can only attach sequence "
            "attributes to vsComponents\n");
        return;
    }

    // Get the number of children
    childCount = ((vsComponent *)theNode)->getChildCount();
    
    // Replace the component's bottom group with a new osg::Sequence group
    osgSequence = new osg::Sequence();
    ((vsComponent *)theNode)->replaceBottomGroup(osgSequence);

    // Set up reasonable defaults for the sequence.  These can be manipulated
    // later
    osgSequence->setMode(osg::Sequence::STOP);
    osgSequence->setInterval(osg::Sequence::SWING, 0, -1);
    osgSequence->setDuration(1.0, -1);
    osgSequence->setMode(osg::Sequence::START);

    // Set the default time for each child to one second
    for (loop = 0; loop < childCount; loop++)
        osgSequence->setTime(loop, 1.0);
    
    // Flag the attribute as attached
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSequenceAttribute::detach(vsNode *theNode)
{
    osg::Group *newGroup;

    // Make sure we're attached to a node, bail out if not
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is not attached\n");
        return;
    }
    
    // Replace the sequence with an ordinary group
    newGroup = new osg::Group();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    osgSequence = NULL;
    
    // Flag the attribute as not attached
    attachedFlag = 0;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsSequenceAttribute::attachDuplicate(vsNode *theNode)
{
    vsSequenceAttribute *newAttrib;
    int loop;
    vsComponent *theComponent;
    
    // Make sure the given node is a component (no other node can
    // receive a sequence attribute)
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;
    
    // Create a new sequence attribute and add it to the node
    newAttrib = new vsSequenceAttribute();
    theNode->addAttribute(newAttrib);

    // Copy all parameters from this sequence to the new one
    for (loop = 0; loop < theComponent->getChildCount(); loop++)    
        newAttrib->setChildTime(loop, getChildTime(loop));
    newAttrib->setRepetitionCount(getRepetitionCount());
    newAttrib->setCycleMode(getCycleMode());
    newAttrib->setPlayMode(getPlayMode());
}
