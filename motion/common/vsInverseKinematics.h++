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
//    VESS Module:  vsInverseKinematics.h++
//
//    Description:  Class for performing inverse kinematics computations
//                  on a chain of vsKinematics objects
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_INVERSE_KINEMATICS_HPP
#define VS_INVERSE_KINEMATICS_HPP

#include "vsKinematics.h++"

struct VS_MOTION_DLL vsInvKinData
{
    vsKinematics *kin;

    vsVector constraintAxis[3];
    double axisMinValue[3];
    double axisMaxValue[3];
};

class VS_MOTION_DLL vsInverseKinematics : public vsObject
{
private:

    // Array of data structures
    vsGrowableArray    dataArray;
    int                dataArraySize;

    // Vector from last joint point to end effector
    vsVector           endpointOffset;

    // Inverse kinematics processing parameters
    int                maxProcessLoops;
    double             successThreshold;
    double             dampeningConstant;

    // Data structure management
    vsInvKinData       *createData();
    void               setDataKinematics(vsInvKinData *data,
                                         vsKinematics *kinematics);
    void               deleteData(vsInvKinData *data);

    // Constraint processing
    double             constrainAngle(double minDegrees, double maxDegrees,
                                      double value);
    double             calculateAxisRotation(vsQuat rotation, vsVector axis);
    vsQuat             applyConstraints(int jointIdx, vsQuat rotation);
    vsQuat             applyDampening(vsQuat rotation, double dampeningFraction);

public:

    // Constructor/destructor
                    vsInverseKinematics();
    virtual         ~vsInverseKinematics();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Kinematics chain management
    void            setKinematicsChainSize(int size);
    int             getKinematicsChainSize();

    void            setKinematicsObject(int jointIdx,
                                        vsKinematics *kinematics);
    vsKinematics    *getKinematicsObject(int jointIdx);

    void            setConstraint(int jointIdx, int axisIdx,
                                  vsVector constraintAxis,
                                  double axisMin, double axisMax);
    void            getConstraint(int jointIdx, int axisIdx,
                                  vsVector *constraintAxis,
                                  double *axisMin, double *axisMax);

    // Other parameters
    void            setEndpointOffset(vsVector offset);
    vsVector        getEndpointOffset();

    void            setMaxLoops(int loops);
    int             getMaxLoops();

    void            setThreshold(double threshold);
    double          getThreshold();

    void            setDampeningConstant(double constant);
    double          getDampeningConstant();

    // Main function
    void            reachForPoint(vsVector targetPoint);
};

#endif
