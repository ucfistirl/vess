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

#ifndef VS_PARTICLE_HPP
#define VS_PARTICLE_HPP

#include "vsUpdatable.h++"
#include "vsParticleSettings.h++"
#include "vsTransformAttribute.h++"
#include "vsGeometry.h++"
#include "vsDynamicGeometry.h++"
#include "atMatrix.h++"
#include "atVector.h++"

class vsParticle : public vsObject
{
protected:

    bool                    active;

    vsComponent             *mainComponent;
    vsTransformAttribute    *positionAttr;
    vsTransformAttribute    *rotScaleAttr;
    vsGeometry              *quadGeometry;
    int                     geomIndex;

    double                  ageSeconds;
    double                  lifetimeSeconds;

    atMatrix                emitterMatrix;

    atVector                position;
    atVector                velocity;

    double                  orbitAngle;
    double                  orbitVelocity;
    double                  orbitRadius;
    double                  orbitRadiusDelta;

    double                  initialSize;
    double                  finalSize;

    double                  rotation;
    double                  rotationSpeed;

    atVector                initialColor;
    atVector                finalColor;

    vsDynamicGeometry       *sharedGeometry;
    int                     primitiveIndex;

    bool                    hardwareShading;

    double                  getRandom();
    double                  getRandomVariance();

public:

                          vsParticle();
    virtual               ~vsParticle();

    virtual const char    *getClassName();

    void                  initSoftware();
    void                  initHardware(vsDynamicGeometry *sharedGeom,
                                       int primIndex);

    void                  setRenderBin(int newBin);

    void                  activate(vsParticleSettings *settings,
                                   atMatrix emitMatrix, atVector initialPos,
                                   vsComponent *parentComponent,
                                   double creationDelay, double frameTime);
    void                  deactivate(vsComponent *parentComponent);

    bool                  isActive();
    int                   getPrimitiveIndex();

    bool                  update(vsParticleSettings *settings,
                                 double deltaTime);
};

#endif

