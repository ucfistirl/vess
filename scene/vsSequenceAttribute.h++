// File vsSequenceAttribute.h++

#ifndef VS_SEQUENCE_ATTRIBUTE_HPP
#define VS_SEQUENCE_ATTRIBUTE_HPP

#include <Performer/pf/pfSequence.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

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

class vsSequenceAttribute : public vsAttribute
{
private:

    pfSequence      *performerSequence;

VS_INTERNAL:

                vsSequenceAttribute(pfSequence *sequenceGroup);

    int         canAttach();
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

public:

                   vsSequenceAttribute();
                   ~vsSequenceAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    void           setChildTime(int childNum, double seconds);
    double         getChildTime(int childNum);

    void           setRepetitionCount(int numReps);
    int            getRepetitionCount();

    void           setCycleMode(int seqCycle);
    int            getCycleMode();

    void           setPlayMode(int playMode);
    int            getPlayMode();

    int            getCurrentChildNum();
};

#endif
