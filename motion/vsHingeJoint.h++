#ifndef VS_HINGE_JOINT_HPP
#define VS_HINGE_JOINT_HPP

// Motion model to support hinge-type articulations (single degree of freedom)
// based on motion tracker data

// Determines how the articulation is calculated:
enum
{
    VS_HINGE_ORIENT_BOTH,
    VS_HINGE_ORIENT_BASE,
    VS_HINGE_ORIENT_DELTA
} vsHingeCalcMethod;


class vsHingeJoint : public vsMotionModel
{
protected:

    // Motion trackers
    vsMotionTracker      *baseTracker;
    vsMotionTracker      *deltaTracker;

    // Offset from tracker position to actual joint position (in tracker 
    // coordinates)
    vsVector             baseJointOffset;
    vsVector             deltaJointOffset;

    // Axis of rotation
    vsVector             rotationAxis;

    // Calculation method
    vsHingeCalcMethod    calcMethod;

public:

                    vsHingeJoint(vsComponent *theComponent, 
                                 vsMotionTracker *base, vsVector baseOffset,
                                 vsMotionTracker *delta, vsVector deltaOffset,
                                 vsVector rotAxis, vsHingeCalcMethod method);

    void            calibrate();

    virtual void    update();
};

#endif
