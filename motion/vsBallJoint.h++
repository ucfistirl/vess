#ifndef VS_BALL_JOINT_HPP
#define VS_BALL_JOINT_HPP

#include "vsMotionModel.h++"
#include "vsMotionTracker.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"

// Motion model to support ball-joint articulations (three degrees of freedom)
// based on motion tracker data

class vsBallJoint : public vsMotionModel
{
protected:

    // Motion trackers
    vsMotionTracker    *upperTracker;
    vsMotionTracker    *lowerTracker;

    // Calculation data
    vsVector           upperJointOffset;
    vsVector           lowerJointOffset;

    vsQuat             lowerTrackerRotOffset;
    vsQuat	       directionXform;
    
    vsQuat	       lastResult;

public:

    // Constructor for one tracker on each side of ball joint
                        vsBallJoint(vsMotionTracker *shoulderTracker,
                                    vsVector shoulderJointOffset,
                                    vsMotionTracker *elbowTracker,
                                    vsVector elbowJointOffset,
                                    vsQuat elbowRotOffset,
				    vsQuat originXform);

    virtual vsMatrix    update();
};

#endif
