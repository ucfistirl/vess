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
    // Initialize the Performer sequence pointer to NULL
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
    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return;
    }

    // Bounds checking
    if (childNum >= performerSequence->getNumChildren())
    {
        printf("vsSequenceAttribute::setChildTime: Index out of bounds\n");
        return;
    }

    // Set the time for the specified child on the Performer object
    performerSequence->setTime(childNum, seconds);
}

// ------------------------------------------------------------------------
// Retrieves the time that the child node with index childNum should be
// displayed for in the sequence. The number of the first child is 0.
// ------------------------------------------------------------------------
double vsSequenceAttribute::getChildTime(int childNum)
{
    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getChildTime: Attribute must be attached "
            "before sequence can be manipulated\n");
        return 0.0;
    }

    // Bounds checking
    if ((childNum < 0) || (childNum >= performerSequence->getNumChildren()))
    {
        printf("vsSequenceAttribute::getChildTime: Index out of bounds\n");
        return 0.0;
    }

    // Set the time for the specified child from the Performer object
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

    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // Sets the number of repetitions of the sequence on the Performer
    // object. Since the setDuration call requires a speed factor as well
    // as a repetition count, get the speed from the sequence first and
    // use that when setting the repetitions.
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

    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getRepetitionCount: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Get the number of repetitions from the Performer sequence
    performerSequence->getDuration(&temp, &result);

    // Return the repetition count
    return result;
}

// ------------------------------------------------------------------------
// Sets the cycle mode for this sequence
// ------------------------------------------------------------------------
void vsSequenceAttribute::setCycleMode(int seqCycle)
{
    int mode, begin, end;

    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // The Performer sequence setInterval function requires the begin
    // and end nodes of the sequence; get those from the sequence so we
    // can pass them back in when setting the cyctem mode.
    performerSequence->getInterval(&mode, &begin, &end);
    
    // Translate the VESS cycle mode constant into a call to the
    // pfSequence setInterval function
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

    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCycleMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Get the cycle mode from the Performer sequence
    performerSequence->getInterval(&mode, &begin, &end);
    
    // Translate the Performer swing mode constant to VESS and return it
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
    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::setPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return;
    }
    
    // Set the pfSequence mode based on the VESS place mode constant
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
    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getPlayMode: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Translate the pfSequence play mode constant into a VESS one
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
    
    // If the Performer constant is unrecognized, return a generic value
    return 0;
}

// ------------------------------------------------------------------------
// Returns the index of the current child being drawn. The index of the
// first child is 0.
// ------------------------------------------------------------------------
int vsSequenceAttribute::getCurrentChildNum()
{
    int temp;

    // Unattached sequences can't be manipulated
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::getCurrentChildNum: Attribute must be "
            "attached before sequence can be manipulated\n");
        return 0;
    }
    
    // Get the current visible child from the vsSequence
    return (performerSequence->getFrame(&temp));
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsSequenceAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
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
    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Sequence attributes may not be attached to geometry nodes
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

    // Set some default values on the pfSequence
    performerSequence->setMode(PFSEQ_STOP);
    performerSequence->setInterval(PFSEQ_SWING, 0, -1);
    performerSequence->setDuration(1.0, -1);
    performerSequence->setTime(-1, 1.0);
    performerSequence->setMode(PFSEQ_START);
    
    // Mark this attribute as attached
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

    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsSequenceAttribute::attach: Attribute is not attached\n");
        return;
    }
    
    // Replace the sequence with an ordinary group
    newGroup = new pfGroup();
    ((vsComponent *)theNode)->replaceBottomGroup(newGroup);
    performerSequence = NULL;
    
    // Mark this attribute as unattached
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
    
    // Verify that the node we're trying to attach the duplicate to
    // is a vsComponent
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
        theComponent = (vsComponent *)theNode;
    else
        return;
    
    // Create a duplicate wireframe attribute
    newAttrib = new vsSequenceAttribute();

    // Attach the duplicate attribute to the specified node first, so that
    // we can manipulate its values
    theNode->addAttribute(newAttrib);

    // Copy the sequence's child durations to the duplicate attribute
    for (loop = 0; loop < theComponent->getChildCount(); loop++)    
        newAttrib->setChildTime(loop, getChildTime(loop));

    // Copy the repetition count, cycle mode, and play mode to the
    // duplicate attribute
    newAttrib->setRepetitionCount(getRepetitionCount());
    newAttrib->setCycleMode(getCycleMode());
    newAttrib->setPlayMode(getPlayMode());
}
