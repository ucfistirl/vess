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
//    VESS Module:  vsParticleSystem.h++
//
//    Description:  Class that constructs a series of objects that behave
//                  as a coherent group
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_PARTICLE_SYSTEM_HPP
#define VS_PARTICLE_SYSTEM_HPP

#include "vsUpdatable.h++"
#include "vsParticle.h++"
#include "vsParticleSettings.h++"
#include "vsArray.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "vsTransformAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsShaderAttribute.h++"
#include "vsGLSLProgramAttribute.h++"
#include "vsTimer.h++"

enum vsParticleSystemEmitterShape
{
    VS_PARTICLESYS_EMITTER_POINT,
    VS_PARTICLESYS_EMITTER_LINE,
    VS_PARTICLESYS_EMITTER_CIRCLE,
    VS_PARTICLESYS_EMITTER_SPHERE,
    VS_PARTICLESYS_EMITTER_SQUARE,
    VS_PARTICLESYS_EMITTER_CUBE
};

enum vsParticleSystemShaderType
{
    VS_PARTICLESYS_ARB_SHADER,
    VS_PARTICLESYS_GLSL_SHADER
};

class VESS_SYM vsParticleSystem : public vsUpdatable
{
protected:

    vsComponent               *parentComponent;

    // Emitter data
    atVector                  emitterPosition;
    atVector                  emitterVelocity;
    atQuat                    emitterOrientation;
    atVector                  emitterAngularVelocityAxis;
    double                    emitterAngularVelocitySpeed;
    vsComponent               *emitterFollowNode;

    double                    emitterAge;
    double                    emitterLifetime;

    double                    emissionRate;
    double                    emissionTimer;

    double                    emitterActive;

    vsParticleSystemEmitterShape    emitterShape;
    double                    emitterMinRadius;
    double                    emitterMaxRadius;

    // Particle list data
    vsArray                   particleList;
    int                       particleListSize;
    int                       activeParticleCount;
    int                       nextInactiveParticleIdx;

    // Global particle data
    vsTextureAttribute        *masterTexture;
    vsTextureAttribute        *tex1;
    vsTextureAttribute        *tex2;
    vsTextureAttribute        *tex3;

    // Hardware shading mode
    vsShaderAttribute         *arbShader;
    vsGLSLProgramAttribute    *glslShader;
    bool                      hardwareShading;
    vsDynamicGeometry         *sharedGeom;
    bool                      *primInUse;

    // Individual particle data
    vsParticleSettings        settings;

    // Follow node extra data
    atVector                  prevFollowNodePos;
    atQuat                    prevFollowNodeOri;
    bool                      prevFollowDataValid;
    atVector                  currentFollowNodePos;
    atQuat                    currentFollowNodeOri;

    vsTimer *                 updateTimer;

    // Particle management routines
    vsParticle                *createParticle();
    void                      destroyParticle(vsParticle *particle);

    void                      activateParticle(vsParticle *particle,
                                               double creationDelay,
                                               double frameTime);
    void                      deactivateParticle(vsParticle *particle);

    void                      updateParticle(vsParticle *particle,
                                             double deltaTime);

    // Utility routines
    double                    getRandom();
    double                    getRandomVariance();
    void                      findNextInactive();

public:

    // 'Structors
                          vsParticleSystem();
                          vsParticleSystem(char *shaderProgram);
                          vsParticleSystem(char *shaderProgram,
                                        vsParticleSystemShaderType shaderType);
                          vsParticleSystem(vsShaderAttribute *shaderAttr);
                          vsParticleSystem(vsGLSLProgramAttribute *shaderAttr);
    virtual               ~vsParticleSystem();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Create a clone of this particle system
    vsParticleSystem      *clone();

    // Inherited from vsUpdatable
    void                  update();
    void                  update(double deltaTime);

    // Particle system main component
    vsComponent           *getComponent();

    // Set the render bin for particles to use
    void                  setRenderBin(int newBin);

    // Particle emission controls
    void                  reset();
    void                  pauseEmission();
    void                  resumeEmission();

    bool                  isEmitterExpired();

    // Emitter parameters get/set
    void                  setEmitterPosition(atVector position);
    atVector              getEmitterPosition();
    void                  setEmitterVelocity(atVector velocity);
    atVector              getEmitterVelocity();
    void                  setEmitterOrientation(atQuat orientation);
    atQuat                getEmitterOrientation();
    void                  setEmitterAngularVelocity(atVector rotationAxis,
                                                    double degreesPerSecond);
    void                  getEmitterAngularVelocity(atVector *rotationAxis,
                                                    double *degreesPerSecond);

    void                  setEmitterFollowComponent(vsComponent *component);
    vsComponent           *getEmitterFollowComponent();

    void                  setEmitterLifetime(double seconds);
    double                getEmitterLifetime();

    void                  setEmitterRate(double particlesPerSecond);
    double                getEmitterRate();

    void                  setEmitterShape(vsParticleSystemEmitterShape shape,
                                          double minRadius,
                                          double maxRadius);
    void                  getEmitterShape(vsParticleSystemEmitterShape *shape,
                                          double *minRadius,
                                          double *maxRadius);

    void                  setMaxParticleCount(int maxParticles);
    int                   getMaxParticleCount();

    // Global particle parameters get/set
    void                  setParticleTexture(char *textureFilename);

    void                  setParticleAcceleration(atVector acceleration);
    atVector              getParticleAcceleration();

    void                  setParticleMaxSpeed(double speed);
    double                getParticleMaxSpeed();

    // Individual particle parameters get/set
    void                  setParticleLifetime(double seconds,
                                              double variance);
    void                  getParticleLifetime(double *seconds,
                                              double *variance);

    void                  setParticleVelocity(atVector velocity,
                                              double minAngleVariance,
                                              double maxAngleVariance,
                                              double speedVariance);
    void                  getParticleVelocity(atVector *velocity,
                                              double *minAngleVariance,
                                              double *maxAngleVariance,
                                              double *speedVariance);

    void                  setParticleOrbitSpeed(double speed,
                                                double variance);
    void                  getParticleOrbitSpeed(double *speed,
                                                double *variance);
    void                  setParticleOrbitRadiusDelta(double speed,
                                                      double variance);
    void                  getParticleOrbitRadiusDelta(double *speed,
                                                      double *variance);

    void                  setParticleSize(double initial,
                                          double initialVariance,
                                          double final,
                                          double finalVariance,
                                          bool uniform);
    void                  getParticleSize(double *initial,
                                          double *initialVariance,
                                          double *final,
                                          double *finalVariance,
                                          bool *uniform);

    void                  setParticleRotation(double rotationDegrees,
                                              double variance);
    void                  getParticleRotation(double *rotationDegrees,
                                              double *variance);
    void                  setParticleRotationSpeed(double degreesPerSecond,
                                                   double variance);
    void                  getParticleRotationSpeed(double *degreesPerSecond,
                                                   double *variance);

    void                  setParticleColor(atVector initial,
                                           atVector initialVariance,
                                           atVector final,
                                           atVector finalVariance,
                                           bool uniformIntra,
                                           bool uniformInter);
    void                  getParticleColor(atVector *initial,
                                           atVector *initialVariance,
                                           atVector *final,
                                           atVector *finalVariance,
                                           bool *uniformIntra,
                                           bool *uniformInter);
};

#endif
