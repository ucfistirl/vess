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
//    VESS Module:  vsPathMotion.h++
//
//    Description:  Motion model that moves an object through a specified
//                  set of key positions and orientations
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_PATH_MOTION_HPP
#define VS_PATH_MOTION_HPP

#include "atVector.h++"
#include "atQuat.h++"
#include "vsArray.h++"
#include "vsKinematics.h++"
#include "vsMotionModel.h++"
#include "vsPathMotionSegment.h++"

#define VS_PATH_WAIT_FOREVER  -1
#define VS_PATH_CYCLE_FOREVER 0

enum  vsPathPosInterpolationMode
{
    VS_PATH_POS_IMODE_NONE,
    VS_PATH_POS_IMODE_LINEAR,
    VS_PATH_POS_IMODE_ROUNDED,
    VS_PATH_POS_IMODE_SPLINE
};

enum  vsPathOrientInterpolationMode
{
    VS_PATH_ORI_IMODE_NONE,
    VS_PATH_ORI_IMODE_SLERP,
    VS_PATH_ORI_IMODE_NLERP,
    VS_PATH_ORI_IMODE_SPLINE,
    VS_PATH_ORI_IMODE_ATPOINT,
    VS_PATH_ORI_IMODE_FORWARD
};

enum  vsPathCycleMode
{
    VS_PATH_CYCLE_RESTART,
    VS_PATH_CYCLE_CLOSED_LOOP
};

enum  vsPathPlayMode
{
    VS_PATH_STOPPED,
    VS_PATH_PAUSED,
    VS_PATH_PLAYING
};

class VESS_SYM vsPathMotion : public vsMotionModel
{
private:

    vsKinematics       *objectKin;

    int                currentPlayMode;

    int                posMode;
    int                oriMode;

    int                cycleMode;
    int                cycleCount;
    int                currentCycleCount;

    double             roundCornerRadius;

    atVector           lookPoint;

    atVector           upDirection;

    int                pointCount;
    vsArray            pointList;

    atVector           currentPos;
    atQuat             currentOri;

    int                currentSegmentIdx;
    double             currentSegmentTime;
    double             totalTime;
    double             totalPathTime;

    double             calcSegLengthLinear(atVector *vec1, atVector *vec2);
    double             calcSegLengthRoundCorner(atVector *vec0, atVector *vec1,
                                                atVector *vec2, atVector *vec3);
    double             calcSegLengthSpline(atVector *vec0, atVector *vec1,
                                           atVector *vec2, atVector *vec3);
    double             calcSubsegLengthSpline(atVector *vec0, atVector *vec1,
                                              atVector *vec2, atVector *vec3,
                                              double start, double end);

    atVector           interpolatePosLinear(atVector *vec1, atVector *vec2,
                                            double parameter);
    atVector           interpolatePosRoundCorner(atVector *vec0, atVector *vec1,
                                                 atVector *vec2, atVector *vec3,
                                                 double parameter);
    atVector           interpolatePosSpline(atVector *vec0, atVector *vec1,
                                            atVector *vec2, atVector *vec3,
                                            double parameter);

    atQuat             interpolateOriSlerp(atQuat *ori1, atQuat *ori2,
                                           double parameter);
    atQuat             interpolateOriNlerp(atQuat *ori1, atQuat *ori2,
                                           double parameter);
    atQuat             interpolateOriSpline(atQuat *ori0, atQuat *ori1,
                                            atQuat *ori2, atQuat *ori3,
                                            double parameter);
    atQuat             interpolateOriToPt(atVector currentPt,
                                          atVector facePt);

    atQuat             quatHalfway(atQuat a, atQuat b, atQuat c);

    vsPathMotionSegment    *getSegmentData(int idx);

public:

                          vsPathMotion(vsKinematics *kinematics);
                          vsPathMotion(vsPathMotion *original);
    virtual               ~vsPathMotion();

    virtual const char    *getClassName();

    void                  setPositionMode(int mode);
    int                   getPositionMode();

    void                  setOrientationMode(int mode);
    int                   getOrientationMode();

    void                  setCycleMode(int mode);
    void                  setCycleCount(int cycles);
    int                   getCycleMode();
    int                   getCycleCount();

    void                  setCornerRadius(double radius);
    double                getCornerRadius();

    void                  setLookAtPoint(atVector point);
    atVector              getLookAtPoint();

    void                  setUpDirection(atVector up);
    atVector              getUpDirection();

    void                  setPointListSize(int size);
    int                   getPointListSize();

    void                  setPosition(int point, atVector position);
    void                  setOrientation(int point, atQuat orientation);
    void                  setTime(int point, double seconds);
    void                  setPauseTime(int point, double seconds);

    atVector              getPosition(int point);
    atQuat                getOrientation(int point);
    double                getTime(int point);
    double                getPauseTime(int point);

    void                  autoSetTimes(double totalPathSeconds);

    void                  startResume();
    void                  pause();
    void                  stop();
    int                   getPlayMode();

    int                   getCurrentSegment();

    void                  configureFromFile(char *filename);

    virtual void          update();
    virtual void          update(double deltaTime);

    atVector              getCurrentPosition();
    atQuat                getCurrentOrientation();

    vsKinematics          *getKinematics();
    void                  setKinematics(vsKinematics *newKin);
};

#endif
