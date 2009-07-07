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
//    VESS Module:  
//
//    Description:  
//
//    Author(s):    
//
//------------------------------------------------------------------------

#ifndef VS_PARTICLE_SETTINGS_HPP
#define VS_PARTICLE_SETTINGS_HPP

#include "vsObject.h++"
#include "atVector.h++"

class vsParticleSettings : public vsObject
{
protected:

    double                    lifetime;
    double                    lifetimeVariance;

    atVector                  initialVelocity;
    double                    velocityMinAngleVariance;
    double                    velocityMaxAngleVariance;
    double                    velocitySpeedVariance;

    atVector                  acceleration;
    double                    maxSpeed;

    double                    orbitSpeed;
    double                    orbitSpeedVariance;
    double                    orbitRadiusDelta;
    double                    orbitRadiusDeltaVariance;

    double                    initialSize;
    double                    initialSizeVariance;
    double                    finalSize;
    double                    finalSizeVariance;
    bool                      lockSizeVariance;
   
    double                    rotation;
    double                    rotationVariance;
    double                    rotationSpeed;
    double                    rotationSpeedVariance;

    atVector                  initialColor;
    atVector                  initialColorVariance;
    atVector                  finalColor;
    atVector                  finalColorVariance;
    bool                      lockIntraColorVariance;
    bool                      lockInterColorVariance;

    int                       renderBin;

public:

                          vsParticleSettings();

    virtual const char    *getClassName();

    void                  setLifetime(double seconds, double var);
    void                  getLifetime(double *seconds, double *var);

    void                  setVelocity(atVector initial, double minAngleVar,
                                      double maxAngleVar, double speedVar);
    void                  getVelocity(atVector *initial, double *minAngleVar,
                                      double *maxAngleVar, double *speedVar);

    void                  setAcceleration(atVector accel);
    atVector              getAcceleration();

    void                  setMaxSpeed(double speed);
    double                getMaxSpeed();

    void                  setOrbit(double speed, double speedVar,
                                   double deltaRadius, double deltaRadiusVar);
    void                  setOrbitSpeed(double speed, double speedVar);
    void                  setOrbitRadiusDelta(double delta, double deltaVar);
    void                  getOrbit(double *speed, double *speedVar,
                                   double *deltaRadius, double *deltaRadiusVar);
    void                  getOrbitSpeed(double *speed, double *speedVar);
    void                  getOrbitRadiusDelta(double *delta, double *deltaVar);

    void                  setSize(double initial, double initialVar,
                                  double final, double finalVar, bool lockVar);
    void                  getSize(double *initial, double *initialVar,
                                  double *final, double *finalVar,
                                  bool *lockVar);

    void                  setRotation(double initialAngle, double angleVar,
                                      double speed, double speedVar);
    void                  setRotationAngle(double angle, double variance);
    void                  setRotationSpeed(double speed, double variance);
    void                  getRotation(double *initialAngle, double *angleVar,
                                      double *speed, double *speedVar);
    void                  getRotationAngle(double *angle, double *variance);
    void                  getRotationSpeed(double *speed, double *variance);

    void                  setColor(atVector initial, atVector initialVar,
                                   atVector final, atVector finalVar,
                                   bool intraLock, bool interLock);
    void                  getColor(atVector *initial, atVector *initialVar,
                                   atVector *final, atVector *finalVar,
                                   bool *intraLock, bool *interLock);

    void                  setRenderBin(int newBin);
    int                   getRenderBin();
};

#endif

