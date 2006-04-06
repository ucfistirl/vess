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
#include "vsComponent.h++"
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

struct vsParticle
{
    bool                    isActive;

    vsComponent             *mainComponent;
    vsTransformAttribute    *positionAttr;
    vsTransformAttribute    *rotScaleAttr;
    vsGeometry              *quadGeometry;
    int                     geomIndex;

    double                  ageSeconds;
    double                  lifetimeSeconds;

    vsMatrix                emitterMatrix;

    vsVector                position;
    vsVector                velocity;

    double                  orbitAngle;
    double                  orbitVelocity;
    double                  orbitRadius;
    double                  orbitRadiusDelta;

    double                  initialSize;
    double                  finalSize;

    double                  rotation;
    double                  rotationSpeed;

    vsVector                initialColor;
    vsVector                finalColor;
};

class VS_GRAPHICS_DLL vsParticleSystem : public vsUpdatable
{
protected:

    vsComponent               *parentComponent;

    // Emitter data
    vsVector                  emitterPosition;
    vsVector                  emitterVelocity;
    vsQuat                    emitterOrientation;
    vsVector                  emitterAngularVelocityAxis;
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
    vsGrowableArray           particleList;
    int                       particleListSize;
    int                       activeParticleCount;
    int                       nextInactiveParticleIdx;

    // Global particle data
    int                       particleRenderBin;
    vsTextureAttribute        *masterTexture;
    vsTextureAttribute        *tex1;
    vsTextureAttribute        *tex2;
    vsTextureAttribute        *tex3;

    vsVector                  globalAcceleration;

    // Hardware shading mode
    vsShaderAttribute         *arbShader;
    vsGLSLProgramAttribute    *glslShader;
    bool                      hardwareShading;
    vsDynamicGeometry         *sharedGeom;
    vsGrowableArray           primInUse;

    // Individual particle data
    double                    lifetime;
    double                    lifetimeVariance;

    vsVector                  initialVelocity;
    double                    velocityMinAngleVariance;
    double                    velocityMaxAngleVariance;
    double                    velocitySpeedVariance;

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

    vsVector                  initialColor;
    vsVector                  initialColorVariance;
    vsVector                  finalColor;
    vsVector                  finalColorVariance;
    bool                      lockIntraColorVariance;
    bool                      lockInterColorVariance;

    // Follow node extra data
    vsVector                  prevFollowNodePos;
    vsQuat                    prevFollowNodeOri;
    bool                      prevFollowDataValid;
    vsVector                  currentFollowNodePos;
    vsQuat                    currentFollowNodeOri;

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
    virtual        ~vsParticleSystem();

    // Inherited from vsObject
    const char     *getClassName();

    // Inherited from vsUpdatable
    void           update();
    void           update(double deltaTime);

    // Particle system main component
    vsComponent    *getComponent();

    // Set the render bin for particles to use
    void           setRenderBin(int newBin);

    // Particle emission controls
    void           reset();
    void           pauseEmission();
    void           resumeEmission();

    bool           isEmitterExpired();

    // Emitter parameters get/set
    void           setEmitterPosition(vsVector position);
    vsVector       getEmitterPosition();
    void           setEmitterVelocity(vsVector velocity);
    vsVector       getEmitterVelocity();
    void           setEmitterOrientation(vsQuat orientation);
    vsQuat         getEmitterOrientation();
    void           setEmitterAngularVelocity(vsVector rotationAxis,
                                             double degreesPerSecond);
    void           getEmitterAngularVelocity(vsVector *rotationAxis,
                                             double *degreesPerSecond);

    void           setEmitterFollowComponent(vsComponent *component);
    vsComponent    *getEmitterFollowComponent();

    void           setEmitterLifetime(double seconds);
    double         getEmitterLifetime();

    void           setEmitterRate(double particlesPerSecond);
    double         getEmitterRate();

    void           setEmitterShape(vsParticleSystemEmitterShape shape,
                                   double minRadius, double maxRadius);
    void           getEmitterShape(vsParticleSystemEmitterShape *shape,
                                   double *minRadius, double *maxRadius);

    void           setMaxParticleCount(int maxParticles);
    int            getMaxParticleCount();

    // Global particle parameters get/set
    void           setParticleTexture(char *textureFilename);

    void           setParticleAcceleration(vsVector acceleration);
    vsVector       getParticleAcceleration();

    // Individual particle parameters get/set
    void           setParticleLifetime(double seconds, double variance);
    void           getParticleLifetime(double *seconds, double *variance);

    void           setParticleVelocity(vsVector velocity,
                                       double minAngleVariance,
                                       double maxAngleVariance,
                                       double speedVariance);
    void           getParticleVelocity(vsVector *velocity,
                                       double *minAngleVariance,
                                       double *maxAngleVariance,
                                       double *speedVariance);

    void           setParticleOrbitSpeed(double speed, double variance);
    void           getParticleOrbitSpeed(double *speed, double *variance);
    void           setParticleOrbitRadiusDelta(double speed, double variance);
    void           getParticleOrbitRadiusDelta(double *speed,
                                               double *variance);

    void           setParticleSize(double initial, double initialVariance,
                                   double final, double finalVariance,
                                   bool uniform);
    void           getParticleSize(double *initial, double *initialVariance,
                                   double *final, double *finalVariance,
                                   bool *uniform);

    void           setParticleRotation(double rotationDegrees,
                                       double variance);
    void           getParticleRotation(double *rotationDegrees,
                                       double *variance);
    void           setParticleRotationSpeed(double degreesPerSecond,
                                            double variance);
    void           getParticleRotationSpeed(double *degreesPerSecond,
                                            double *variance);

    void           setParticleColor(vsVector initial,
                                    vsVector initialVariance, vsVector final,
                                    vsVector finalVariance, bool uniformIntra,
                                    bool uniformInter);
    void           getParticleColor(vsVector *initial,
                                    vsVector *initialVariance,
                                    vsVector *final, vsVector *finalVariance,
                                    bool *uniformIntra, bool *uniformInter);
};

#endif
