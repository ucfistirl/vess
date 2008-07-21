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
//                  vsComponent with a vsSequenceAttribute attached.
//                  This is necessary to emulate Performer's
//                  behavior when a negative time is set on a sequence
//                  frame (this pauses the sequence).
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SEQUENCE_CALLBACK_HPP
#define VS_SEQUENCE_CALLBACK_HPP

class vsSequenceCallback;

#include "vsSequenceAttribute.h++"
#include <osg/NodeCallback>

class VESS_SYM vsSequenceCallback : public osg::NodeCallback
{
private:

    vsSequenceAttribute    *sequenceAttr;
    int                    lastFrameNumber;

public:

                    vsSequenceCallback(vsSequenceAttribute *seqAttr);
    virtual         ~vsSequenceCallback();

    virtual void    operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif
