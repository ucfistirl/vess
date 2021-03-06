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
//    VESS Module:  vsSequenceAttribute.h++
//
//    Description:  Attribute that specifies that the children of the
//                  component are multiple frames of an animation
//                  sequence and should be drawn sequentially for the
//                  specified periods of time rather than all at once.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SEQUENCE_ATTRIBUTE_HPP
#define VS_SEQUENCE_ATTRIBUTE_HPP

class vsSequenceAttribute;

#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsSequenceCallback.h++"
#include <osg/Sequence>

#define VS_SEQUENCE_ALL_CHILDREN -1
#define VS_SEQUENCE_TIME_PAUSE   -1.0

enum vsSequenceCycle
{
    VS_SEQUENCE_CYCLE_FORWARD,
    VS_SEQUENCE_CYCLE_SWING
};

enum vsSequenceMode
{
    VS_SEQUENCE_MODE_START,
    VS_SEQUENCE_MODE_STOP,
    VS_SEQUENCE_MODE_PAUSE,
    VS_SEQUENCE_MODE_RESUME
};

class VESS_SYM vsSequenceAttribute : public vsAttribute
{
private:

    osg::Sequence         *osgSequence;
    vsSequenceCallback    *seqCallback;

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

                           vsSequenceAttribute();
    virtual                ~vsSequenceAttribute();

    virtual const char     *getClassName();

    virtual int            getAttributeType();
    virtual int            getAttributeCategory();
    virtual vsAttribute    *clone();

    void                   setChildTime(int childNum, double seconds);
    double                 getChildTime(int childNum);

    void                   setRepetitionCount(int numReps);
    int                    getRepetitionCount();

    void                   setCycleMode(int seqCycle);
    int                    getCycleMode();

    void                   setPlayMode(int playMode);
    int                    getPlayMode();

    int                    getCurrentChildNum();
};

#endif
