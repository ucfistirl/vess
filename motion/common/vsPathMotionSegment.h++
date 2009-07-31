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
//    VESS Module:  vsPathMotionSegment.h++
//
//    Description:  Stores the data for a point on a vsPathMotion path
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsObject.h++"
#include "atVector.h++"
#include "atQuat.h++"

class vsPathMotionSegment : public vsObject
{
protected:

    atVector   position;
    atQuat     orientation;
    double     travelTime;
    double     pauseTime;

public:

                          vsPathMotionSegment();
    virtual               ~vsPathMotionSegment();

    virtual const char    *getClassName();

    vsPathMotionSegment   *clone();

    virtual void          setPosition(atVector pos);
    virtual atVector      getPosition();

    virtual void          setOrientation(atQuat orient);
    virtual atQuat        getOrientation();

    virtual void          setTravelTime(double seconds);
    virtual double        getTravelTime();

    virtual void          setPauseTime(double seconds);
    virtual double        getPauseTime();
};

