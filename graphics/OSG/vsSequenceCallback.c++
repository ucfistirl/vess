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
//    VESS Module:  vsSequenceCallback.h++
//
//    Description:  OSG-specific class that implements a callback which
//                  is called when an OSG cull traversal reaches a
//                  vsComponent with a vsSequenceAttribute attached
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <osgUtil/CullVisitor>
#include "vsSequenceCallback.h++"
#include "vsSequenceAttribute.h++"

//------------------------------------------------------------------------
// Constructor
// Stores the pointer to the parent billboard attribute
//------------------------------------------------------------------------
vsSequenceCallback::vsSequenceCallback(vsSequenceAttribute *seqAttr)
{
    // Store the sequence attribute
    sequenceAttr = seqAttr;

    // Initialize the lastFrameNumber variable to an impossible value.
    // This makes sure the first call to the callback actually does
    // something.
    lastFrameNumber = -1;
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsSequenceCallback::~vsSequenceCallback()
{
}

//------------------------------------------------------------------------
// OSG callback function
// Called when a cull traversal reaches a vsComponent with a sequence
// attribute attached.  Checks the frame time on the current child and
// pauses the sequence if the frame time is negative.
//------------------------------------------------------------------------
void vsSequenceCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    int frameNumber;
    double frameTime;

    // Get the current frame number
    frameNumber = sequenceAttr->getCurrentChildNum();

    // In OSG, Sequence is a subclass of Switch, so it can in theory
    // be set to have all of its children on or off, or to have multiple
    // children on.  If this is the case, don't bother checking for
    // a negative frame time
    if ((frameNumber != osg::Switch::ALL_CHILDREN_ON) &&
        (frameNumber != osg::Switch::ALL_CHILDREN_OFF) &&
        (frameNumber != osg::Switch::MULTIPLE_CHILDREN_ON))
    {
        // Don't do anything if the sequence isn't playing
        if (sequenceAttr->getPlayMode() == VS_SEQUENCE_MODE_START)
        {
            // Get the current child's frame time
            frameTime = sequenceAttr->getChildTime(frameNumber);

            // If the current sequence frame is the same as the frame
            // during the last call to this callback, don't do anything.
            // This prevents the sequence from re-pausing after a resume.
            if (lastFrameNumber != frameNumber)
            {
                // If the child time is negative, pause the sequence
                if (frameTime < 0.0)
                {
                    sequenceAttr->setPlayMode(VS_SEQUENCE_MODE_PAUSE);
                }
            }
        }
    }

    // Remember the current frame number for next time
    lastFrameNumber = frameNumber;

    // Continue the cull traversal
    traverse(node,nv);
}
