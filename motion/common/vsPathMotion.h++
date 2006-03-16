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

#include "vsVector.h++"
#include "vsQuat.h++"
#include "vsKinematics.h++"
#include "vsMotionModel.h++"

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

struct VS_MOTION_DLL vsPathMotionSegment
{
    vsVector    position;
    vsQuat      orientation;
    double      travelTime;
    double      pauseTime;
};

class VS_MOTION_DLL vsPathMotion : public vsMotionModel
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

    vsVector           lookPoint;

    vsVector           upDirection;

    int                pointCount;
    vsGrowableArray    pointList;

    vsVector           currentPos;
    vsQuat             currentOri;

    int                currentSegmentIdx;
    double             currentSegmentTime;
    double             totalTime;
    double             totalPathTime;

    double             calcSegLengthLinear(vsVector *vec1, vsVector *vec2);
    double             calcSegLengthRoundCorner(vsVector *vec0, vsVector *vec1,
                                                vsVector *vec2, vsVector *vec3);
    double             calcSegLengthSpline(vsVector *vec0, vsVector *vec1,
                                           vsVector *vec2, vsVector *vec3);
    double             calcSubsegLengthSpline(vsVector *vec0, vsVector *vec1,
                                              vsVector *vec2, vsVector *vec3,
                                              double start, double end);

    vsVector           interpolatePosLinear(vsVector *vec1, vsVector *vec2,
                                            double parameter);
    vsVector           interpolatePosRoundCorner(vsVector *vec0, vsVector *vec1,
                                                 vsVector *vec2, vsVector *vec3,
                                                 double parameter);
    vsVector           interpolatePosSpline(vsVector *vec0, vsVector *vec1,
                                            vsVector *vec2, vsVector *vec3,
                                            double parameter);

    vsQuat             interpolateOriSlerp(vsQuat *ori1, vsQuat *ori2,
                                           double parameter);
    vsQuat             interpolateOriSpline(vsQuat *ori0, vsQuat *ori1,
                                            vsQuat *ori2, vsQuat *ori3,
                                            double parameter);
    vsQuat             interpolateOriToPt(vsVector currentPt,
                                          vsVector facePt);

    vsQuat             quatHalfway(vsQuat a, vsQuat b, vsQuat c);

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

    void                  setLookAtPoint(vsVector point);
    vsVector              getLookAtPoint();

    void                  setUpDirection(vsVector up);
    vsVector              getUpDirection();

    void                  setPointListSize(int size);
    int                   getPointListSize();

    void                  setPosition(int point, vsVector position);
    void                  setOrientation(int point, vsQuat orientation);
    void                  setTime(int point, double seconds);
    void                  setPauseTime(int point, double seconds);

    vsVector              getPosition(int point);
    vsQuat                getOrientation(int point);
    double                getTime(int point);
    double                getPauseTime(int point);

    void                  autoSetTimes(double totalPathSeconds);

    void                  startResume();
    void                  pause();
    void                  stop();
    int                   getPlayMode();

    void                  configureFromFile(char *filename);

    virtual void          update();
    virtual void          update(double deltaTime);

    vsVector              getCurrentPosition();
    vsQuat                getCurrentOrientation();

    void                  setKinematics(vsKinematics *newKin);
};

#endif
