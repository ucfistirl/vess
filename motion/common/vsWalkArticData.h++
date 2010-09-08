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
//    VESS Module:  vsWalkArticData.h++
//
//    Description:  Class to hold a single keyframe position for the
//                  vsWalkArticulation motion model.
//
//    Author(s):    Jason Daly, Bryan Kline
//
//------------------------------------------------------------------------

#include "atQuat.h++"
#include "vsObject.h++"

class vsWalkArticData : public vsObject
{
protected:

    atQuat   leftHip;
    atQuat   leftKnee;
    atQuat   leftAnkle;
    atQuat   rightHip;
    atQuat   rightKnee;
    atQuat   rightAnkle;
    double   distance;

public:

                              vsWalkArticData();     
                              vsWalkArticData(atQuat lhip, atQuat lknee,
                                              atQuat lankle, atQuat rhip,
                                              atQuat rknee, atQuat rankle,
                                              double dist);
    virtual                   ~vsWalkArticData();

    virtual const char        *getClassName();

    virtual vsWalkArticData   *clone();

    virtual atQuat            getLeftHip();
    virtual void              setLeftHip(atQuat newRotation);

    virtual atQuat            getLeftKnee();
    virtual void              setLeftKnee(atQuat newRotation);

    virtual atQuat            getLeftAnkle();
    virtual void              setLeftAnkle(atQuat newRotation);

    virtual atQuat            getRightHip();
    virtual void              setRightHip(atQuat newRotation);

    virtual atQuat            getRightKnee();
    virtual void              setRightKnee(atQuat newRotation);

    virtual atQuat            getRightAnkle();
    virtual void              setRightAnkle(atQuat newRotation);

    virtual double            getDistance();
    virtual void              setDistance(double distance);
};

