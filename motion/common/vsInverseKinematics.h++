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

class VS_MOTION_DLL vsInverseKinematics : public vsObject
{
private:

    // Array of kinematics objects
    vsGrowableArray    kinematicsArray;
    int                kinematicsArraySize;

    // Vector from last joint point to end effector
    atVector           endpointOffset;

    // Inverse kinematics processing parameters
    int                maxProcessLoops;
    double             successThreshold;
    double             dampeningConstant;

    // Angle processing
    atQuat             applyDampening(atQuat rotation, double dampeningFraction);

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

    // Other parameters
    void            setEndpointOffset(atVector offset);
    atVector        getEndpointOffset();

    void            setMaxLoops(int loops);
    int             getMaxLoops();

    void            setThreshold(double threshold);
    double          getThreshold();

    void            setDampeningConstant(double constant);
    double          getDampeningConstant();

    // Main function
    void            reachForPoint(atVector targetPoint);
};

#endif
