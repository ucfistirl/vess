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
//    VESS Module:  vsHandCollision.h++
//
//    Description:  A class to create an manage an array of spherical
//                  sensors used to detect collisions between a human
//                  hand model and the environment.  Includes support
//                  for detecting a grasp of an object.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_HAND_COLLISION_HPP
#define VS_HAND_COLLISION_HPP

#include "vsObject.h++"
#include "vsSphereIntersect.h++"
#include "vsComponent.h++"
#include "vsMaterialAttribute.h++"

#define VS_HC_MAX_SENSORS 32

#define VS_HC_DEFAULT_ISECT_MASK       0xFFFFFFFE
#define VS_HC_DEFAULT_HAND_ISECT_VALUE 0x00000001

class VS_MOTION_DLL vsHandCollision : public vsObject
{
protected:

    int                    numSensors;
    vsComponent            *sceneComp;
    vsComponent            *handComp;
    vsComponent            *sensorComp[VS_HC_MAX_SENSORS];
    double                 sensorRadius[VS_HC_MAX_SENSORS];

    bool                   highlightEnabled;
    vsComponent            *handSegment[VS_HC_MAX_SENSORS];
    vsMaterialAttribute    *oldMaterial[VS_HC_MAX_SENSORS];
    vsMaterialAttribute    *highlightMaterial;

    vsSphereIntersect      *sphIsect;

    int                    sensorMask;
    int                    fingerMask, thumbMask;
    int                    firstThumbSensor, lastThumbSensor;

    unsigned int           collisionState;

    void                   init();
    void                   loadConfiguration(FILE *fp);

public:

                          vsHandCollision(char *configFileName, 
                                          vsComponent *scene);
                          vsHandCollision(int nSensors, vsComponent *scene,
                                          vsComponent *hand,
                                          vsComponent *sensors[],
                                          double radius[],
                                          int thumbMin, int thumbMax,
                                          vsComponent *handSeg[]);
    virtual               ~vsHandCollision();

    virtual const char    *getClassName();

    void                  setIntersectMask(unsigned long mask);
    void                  setHandIntersectValue(unsigned long value);

    bool                  isColliding(int sensorIndex);
    bool                  isGraspingObject(vsComponent *object);
    unsigned long         getCollisionState();

    bool                  getIsectValid(int sensorIndex);
    atVector              getIsectPoint(int sensorIndex);
    atVector              getIsectNorm(int sensorIndex);
    atMatrix              getIsectXform(int sensorIndex);
    vsGeometry            *getIsectGeometry(int sensorIndex);
    int                   getIsectPrimNum(int sensorIndex);
    vsGrowableArray       *getIsectPath(int sensorIndex);

    void                  enableHighlighting();
    void                  disableHighlighting();

    void                  update();
};

#endif
