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

#include "vsSequenceAttribute.h++"

#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Default Constructor
// ------------------------------------------------------------------------
vsSequenceAttribute::vsSequenceAttribute()
{
    performerSequence = NULL;
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
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return;
    }

    if (childNum >= performerSequence->getNumChildren())
    {
        printf("vsSequenceAttribute::setChildTime: Index out of bounds\n");
        return;
    }

    performerSequence->setTime(childNum, seconds);
}

// ------------------------------------------------------------------------
// Retrieves the time that the child node with index childNum should be
// displayed for in the sequence. The number of the first child is 0.
// ------------------------------------------------------------------------
double vsSequenceAttribute::getChildTime(int childNum)
{
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return 0.0;
    }

    if ((childNum < 0) || (childNum >= performerSequence->getNumChildren()))
    {
        printf("vsSequenceAttribute::getChildTime: Index out of bounds\n");
        return 0.0;
    }

    return (performerSequence->getTime(childNum));
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

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    performerSequence->getDuration(&speed, &temp);
    performerSequence->setDuration(speed, numReps);
}

// ------------------------------------------------------------------------
// Retrieves the number of times that this sequence should repeat itself
// ------------------------------------------------------------------------
int vsSequenceAttribute::getRepetitionCount()
{
    int result;
    float temp;

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    performerSequence->getDuration(&temp, &result);

    return result;
}

// ------------------------------------------------------------------------
// Sets the cycle mode for this sequence
// ------------------------------------------------------------------------
void vsSequenceAttribute::setCycleMode(int seqCycle)
{
    int mode, begin, end;

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    performerSequence->getInterval(&mode, &begin, &end);
    
    switch (seqCycle)
    {
        case VS_SEQUENCE_CYCLE_FORWARD:
            performerSequence->setInterval(PFSEQ_CYCLE, begin, end);
            break;
        case VS_SEQUENCE_CYCLE_SWING:
            performerSequence->setInterval(PFSEQ_SWING, begin, end);
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
    int mode, begin, end;

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    performerSequence->getInterval(&mode, &begin, &end);
    
    if (mode == PFSEQ_SWING)
        return VS_SEQUENCE_CYCLE_SWING;
    else
        return VS_SEQUENCE_CYCLE_FORWARD;
}

// ------------------------------------------------------------------------
// Starts or stops the sequence playing
// ------------------------------------------------------------------------
void vsSequenceAttribute::setPlayMode(int playMode)
{
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    switch (playMode)
    {
        case VS_SEQUENCE_MODE_START:
            performerSequence->setMode(PFSEQ_START);
            break;
        case VS_SEQUENCE_MODE_STOP:
            performerSequence->setMode(PFSEQ_STOP);
            break;
        case VS_SEQUENCE_MODE_PAUSE:
            performerSequence->setMode(PFSEQ_PAUSE);
            break;
        case VS_SEQUENCE_MODE_RESUME:
            performerSequence->setMode(PFSEQ_RESUME);
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
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    switch (performerSequence->getMode())
    {
        case PFSEQ_START:
            return VS_SEQUENCE_MODE_START;
        case PFSEQ_STOP:
            return VS_SEQUENCE_MODE_STOP;
        case PFSEQ_PAUSE:
            return VS_SEQUENCE_MODE_PAUSE;
        case PFSEQ_RESUME:
            return VS_SEQUENCE_MODE_RESUME;
    }
    
    return 0;
}

// ------------------------------------------------------------------------
// Returns the index of the current child being drawn. The index of the
// first child is 0.
// ------------------------------------------------------------------------
int vsSequenceAttribute::getCurrentChildNum()
{
    int temp;

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCurrentChildNum: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    return (performerSequence->getFrame(&temp));
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsSequenceAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSequenceAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is already attached\n");
        return;
    }

    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsSequenceAttribute::attach: Can't attach sequence attributes "
            "to geometry nodes\n");
        return;
    }
    
    // Replace the bottom group with a sequence group
    performerSequence = new pfSequence();
    ((vsComponent *)theNode)->replaceBottomGroup(performerSequence);

    performerSequence->setMode(PFSEQ_STOP);
    performerSequence->setInterval(PFSEQ_SWING, 0, -1);
    performerSequence->setDuration(1.0, -1);
    performerSequence->setTime(-1, 1.0);
    performerSequence->setMode(PFSEQ_START);
    
    attachedFlag = 1;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsSequenceAttribute::detach(vsNode *theNode)
{
    pfGroup *newGroup;

    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is not attached\n");
        return;
    }
    
    // Replace the sequence with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerSequence = NULL;
    
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
    
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;
    
    newAttrib = new vsSequenceAttribute();

    theNode->addAttribute(newAttrib);

    for (loop = 0; loop < theComponent->getChildCount(); loop++)    
        newAttrib->setChildTime(loop, getChildTime(loop));
    newAttrib->setRepetitionCount(getRepetitionCount());
    newAttrib->setCycleMode(getCycleMode());
    newAttrib->setPlayMode(getPlayMode());
}