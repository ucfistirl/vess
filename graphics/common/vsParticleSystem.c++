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

#include "vsParticleSystem.h++"

#include "vsBillboardAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsParticleSystem::vsParticleSystem() 
                : particleList(0, 1), primInUse(0, 1)
{
    vsTransparencyAttribute *transpAttr;

    // Create the master component
    parentComponent = new vsComponent();
    parentComponent->ref();

    // Not using hardware shading, set the flag and shader variable
    // appropriately.  Also set the "shared" geometry node to NULL, since
    // each particle will have its own geometry node
    hardwareShading = false;
    arbShader = NULL;
    glslShader = NULL;
    sharedGeom = NULL;

    // Create the texture attribute that all the particles will use
    masterTexture = new vsTextureAttribute();
    masterTexture->ref();
    masterTexture->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    parentComponent->addAttribute(masterTexture);

    // Add transparency to the particles
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    transpAttr->disableOcclusion();
    parentComponent->addAttribute(transpAttr);

    // Set the renderbin for each particle to 0 (the default bin)
    particleRenderBin = 0;

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    setMaxParticleCount(10);
    nextInactiveParticleIdx = 0;

    // Emitter default paramters
    emitterPosition.set(0.0, 0.0, 0.0);
    emitterVelocity.set(0.0, 0.0, 0.0);
    emitterOrientation.set(0.0, 0.0, 0.0, 1.0);
    emitterAngularVelocityAxis.set(0.0, 0.0, 1.0);
    emitterAngularVelocitySpeed = 0.0;
    emitterFollowNode = NULL;

    emitterAge = 0.0;
    emitterLifetime = -1.0;

    emissionRate = 1.0;
    emissionTimer = 0.0;

    emitterActive = true;

    emitterShape = VS_PARTICLESYS_EMITTER_SPHERE;
    emitterMinRadius = 0.0;
    emitterMaxRadius = 0.0;

    // Particle default parameters
    globalAcceleration.set(0.0, 0.0, 0.0);

    lifetime = 1.0;
    lifetimeVariance = 0.0;

    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Mark that the 'previous' follow node data isn't valid yet
    prevFollowDataValid = false;
    
    // Create and mark the update timer.
    updateTimer = new vsTimer();
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Constructor, creates a particle system in hardware mode, using the
// ARB_vertex_program code in the specified file
// ------------------------------------------------------------------------
vsParticleSystem::vsParticleSystem(char *shaderProgram)
                : particleList(0, 1), primInUse(0, 1)
{
    vsTransparencyAttribute *transpAttr;

    // Create the master component
    parentComponent = new vsComponent();
    parentComponent->ref();

    // Initialize the shader attributes to NULL
    arbShader = NULL;
    glslShader = NULL;

    // Create the shader attribute
    hardwareShading = true;
    arbShader = new vsShaderAttribute();
    arbShader->setVertexSourceFile(shaderProgram);
    arbShader->ref();
    parentComponent->addAttribute(arbShader);

    // Create the texture attribute that all the particles will use
    masterTexture = new vsTextureAttribute();
    masterTexture->ref();
    masterTexture->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    parentComponent->addAttribute(masterTexture);

    // Add transparency to the particles
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    transpAttr->disableOcclusion();
    parentComponent->addAttribute(transpAttr);

    // Set the renderbin for each particle to 0 (the default bin)
    particleRenderBin = 0;

    // Create the shared geometry node that all particles will use
    sharedGeom = new vsDynamicGeometry();
    sharedGeom->ref();
    sharedGeom->beginNewState();
    sharedGeom->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
    sharedGeom->enableLighting();
    sharedGeom->disableCull();
    sharedGeom->setIntersectValue(0x00000001);
    sharedGeom->setBinding(VS_GEOMETRY_VERTEX_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_NORMALS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_COLORS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE0_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE1_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE2_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE3_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->finishNewState();
    parentComponent->addChild(sharedGeom);

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    setMaxParticleCount(10);
    nextInactiveParticleIdx = 0;

    // Emitter default paramters
    emitterPosition.set(0.0, 0.0, 0.0);
    emitterVelocity.set(0.0, 0.0, 0.0);
    emitterOrientation.set(0.0, 0.0, 0.0, 1.0);
    emitterAngularVelocityAxis.set(0.0, 0.0, 1.0);
    emitterAngularVelocitySpeed = 0.0;
    emitterFollowNode = NULL;

    emitterAge = 0.0;
    emitterLifetime = -1.0;

    emissionRate = 1.0;
    emissionTimer = 0.0;

    emitterActive = true;

    emitterShape = VS_PARTICLESYS_EMITTER_SPHERE;
    emitterMinRadius = 0.0;
    emitterMaxRadius = 0.0;

    // Particle default parameters
    globalAcceleration.set(0.0, 0.0, 0.0);

    lifetime = 1.0;
    lifetimeVariance = 0.0;

    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Mark that the 'previous' follow node data isn't valid yet
    prevFollowDataValid = false;
    
    // Create and mark the update timer.
    updateTimer = new vsTimer();
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Constructor, creates a particle system in hardware mode, using the
// source code for the specified type of shader in the specified file.
// ------------------------------------------------------------------------
vsParticleSystem::vsParticleSystem(char *shaderProgram, 
                                   vsParticleSystemShaderType shaderType)
                : particleList(0, 1), primInUse(0, 1)
{
    vsTransparencyAttribute *transpAttr;
    vsGLSLShader *shaderObject;

    // Create the master component
    parentComponent = new vsComponent();
    parentComponent->ref();

    // Initialize the shader attributes to NULL
    arbShader = NULL;
    glslShader = NULL;

    // Create the shader attribute
    switch (shaderType)
    {
        case VS_PARTICLESYS_ARB_SHADER:
            hardwareShading = true;
            arbShader = new vsShaderAttribute();
            arbShader->setVertexSourceFile(shaderProgram);
            arbShader->ref();
            parentComponent->addAttribute(arbShader);
            break;

        case VS_PARTICLESYS_GLSL_SHADER:
            hardwareShading = true;
            glslShader = new vsGLSLProgramAttribute();
            glslShader->ref();
            shaderObject = new vsGLSLShader(VS_GLSL_VERTEX_SHADER);
            shaderObject->setSourceFile(shaderProgram);
            glslShader->addShader(shaderObject);
            parentComponent->addAttribute(glslShader);
            break;

        default:
            printf("vsParticleSystem::vsParticleSystem: Unknown shader type "
                "specified!\n");
    }

    // Create the texture attribute that all the particles will use
    masterTexture = new vsTextureAttribute();
    masterTexture->ref();
    masterTexture->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    parentComponent->addAttribute(masterTexture);

    // Add transparency to the particles
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    transpAttr->disableOcclusion();
    parentComponent->addAttribute(transpAttr);

    // Set the renderbin for each particle to 0 (the default bin)
    particleRenderBin = 0;

    // Create the shared geometry node that all particles will use
    sharedGeom = new vsDynamicGeometry();
    sharedGeom->ref();
    sharedGeom->beginNewState();
    sharedGeom->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
    sharedGeom->enableLighting();
    sharedGeom->disableCull();
    sharedGeom->setIntersectValue(0x00000001);
    sharedGeom->setBinding(VS_GEOMETRY_VERTEX_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_NORMALS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_COLORS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE0_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE1_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE2_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE3_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->finishNewState();
    parentComponent->addChild(sharedGeom);

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    setMaxParticleCount(10);
    nextInactiveParticleIdx = 0;

    // Emitter default paramters
    emitterPosition.set(0.0, 0.0, 0.0);
    emitterVelocity.set(0.0, 0.0, 0.0);
    emitterOrientation.set(0.0, 0.0, 0.0, 1.0);
    emitterAngularVelocityAxis.set(0.0, 0.0, 1.0);
    emitterAngularVelocitySpeed = 0.0;
    emitterFollowNode = NULL;

    emitterAge = 0.0;
    emitterLifetime = -1.0;

    emissionRate = 1.0;
    emissionTimer = 0.0;

    emitterActive = true;

    emitterShape = VS_PARTICLESYS_EMITTER_SPHERE;
    emitterMinRadius = 0.0;
    emitterMaxRadius = 0.0;

    // Particle default parameters
    globalAcceleration.set(0.0, 0.0, 0.0);

    lifetime = 1.0;
    lifetimeVariance = 0.0;

    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Mark that the 'previous' follow node data isn't valid yet
    prevFollowDataValid = false;
    
    // Create and mark the update timer.
    updateTimer = new vsTimer();
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Constructor, creates a particle system in hardware mode, using the
// given pre-existing ARB shader attribute.
// ------------------------------------------------------------------------
vsParticleSystem::vsParticleSystem(vsShaderAttribute *shaderAttr)                                    
                : particleList(0, 1), primInUse(0, 1)
{
    vsTransparencyAttribute *transpAttr;

    // Create the master component
    parentComponent = new vsComponent();
    parentComponent->ref();

    // Initialize the shader attributes
    arbShader = shaderAttr;
    glslShader = NULL;
    parentComponent->addAttribute(arbShader);
    arbShader->ref();
    hardwareShading = true;

    // Create the texture attribute that all the particles will use
    masterTexture = new vsTextureAttribute();
    masterTexture->ref();
    masterTexture->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    parentComponent->addAttribute(masterTexture);

    // Add transparency to the particles
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    transpAttr->disableOcclusion();
    parentComponent->addAttribute(transpAttr);

    // Set the renderbin for each particle to 0 (the default bin)
    particleRenderBin = 0;

    // Create the shared geometry node that all particles will use
    sharedGeom = new vsDynamicGeometry();
    sharedGeom->ref();
    sharedGeom->beginNewState();
    sharedGeom->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
    sharedGeom->enableLighting();
    sharedGeom->disableCull();
    sharedGeom->setIntersectValue(0x00000001);
    sharedGeom->setBinding(VS_GEOMETRY_VERTEX_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_NORMALS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_COLORS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE0_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE1_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE2_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE3_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->finishNewState();
    parentComponent->addChild(sharedGeom);

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    setMaxParticleCount(10);
    nextInactiveParticleIdx = 0;

    // Emitter default paramters
    emitterPosition.set(0.0, 0.0, 0.0);
    emitterVelocity.set(0.0, 0.0, 0.0);
    emitterOrientation.set(0.0, 0.0, 0.0, 1.0);
    emitterAngularVelocityAxis.set(0.0, 0.0, 1.0);
    emitterAngularVelocitySpeed = 0.0;
    emitterFollowNode = NULL;

    emitterAge = 0.0;
    emitterLifetime = -1.0;

    emissionRate = 1.0;
    emissionTimer = 0.0;

    emitterActive = true;

    emitterShape = VS_PARTICLESYS_EMITTER_SPHERE;
    emitterMinRadius = 0.0;
    emitterMaxRadius = 0.0;

    // Particle default parameters
    globalAcceleration.set(0.0, 0.0, 0.0);

    lifetime = 1.0;
    lifetimeVariance = 0.0;

    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Mark that the 'previous' follow node data isn't valid yet
    prevFollowDataValid = false;
    
    // Create and mark the update timer.
    updateTimer = new vsTimer();
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Constructor, creates a particle system in hardware mode, using the
// given GLSL program attribute (thus, avoiding the cost of a recompile
// ------------------------------------------------------------------------
vsParticleSystem::vsParticleSystem(vsGLSLProgramAttribute *shaderAttr)                                    
                : particleList(0, 1), primInUse(0, 1)
{
    vsTransparencyAttribute *transpAttr;

    // Create the master component
    parentComponent = new vsComponent();
    parentComponent->ref();

    // Initialize the shader attributes
    arbShader = NULL;
    glslShader = shaderAttr;
    parentComponent->addAttribute(glslShader);
    glslShader->ref();
    hardwareShading = true;

    // Create the texture attribute that all the particles will use
    masterTexture = new vsTextureAttribute();
    masterTexture->ref();
    masterTexture->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    parentComponent->addAttribute(masterTexture);

    // Add transparency to the particles
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    transpAttr->disableOcclusion();
    parentComponent->addAttribute(transpAttr);

    // Set the renderbin for each particle to 0 (the default bin)
    particleRenderBin = 0;

    // Create the shared geometry node that all particles will use
    sharedGeom = new vsDynamicGeometry();
    sharedGeom->ref();
    sharedGeom->beginNewState();
    sharedGeom->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
    sharedGeom->enableLighting();
    sharedGeom->disableCull();
    sharedGeom->setIntersectValue(0x00000001);
    sharedGeom->setBinding(VS_GEOMETRY_VERTEX_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_NORMALS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_COLORS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE0_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE1_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE2_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->setBinding(VS_GEOMETRY_TEXTURE3_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    sharedGeom->finishNewState();
    parentComponent->addChild(sharedGeom);

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    setMaxParticleCount(10);
    nextInactiveParticleIdx = 0;

    // Emitter default paramters
    emitterPosition.set(0.0, 0.0, 0.0);
    emitterVelocity.set(0.0, 0.0, 0.0);
    emitterOrientation.set(0.0, 0.0, 0.0, 1.0);
    emitterAngularVelocityAxis.set(0.0, 0.0, 1.0);
    emitterAngularVelocitySpeed = 0.0;
    emitterFollowNode = NULL;

    emitterAge = 0.0;
    emitterLifetime = -1.0;

    emissionRate = 1.0;
    emissionTimer = 0.0;

    emitterActive = true;

    emitterShape = VS_PARTICLESYS_EMITTER_SPHERE;
    emitterMinRadius = 0.0;
    emitterMaxRadius = 0.0;

    // Particle default parameters
    globalAcceleration.set(0.0, 0.0, 0.0);

    lifetime = 1.0;
    lifetimeVariance = 0.0;

    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Mark that the 'previous' follow node data isn't valid yet
    prevFollowDataValid = false;
    
    // Create and mark the update timer.
    updateTimer = new vsTimer();
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsParticleSystem::~vsParticleSystem()
{
    int loop;

    // Destroy all of the particles in the list
    for (loop = 0; loop < particleListSize; loop++)
        destroyParticle((vsParticle *)(particleList[loop]));

    // De-reference any shader attributes we created or used
    if (arbShader)
    {
        parentComponent->removeAttribute(arbShader);
        arbShader->unref();
    }
    if (glslShader)
    {
        parentComponent->removeAttribute(glslShader);
        glslShader->unref();
    }
    
    // If we're using shared geometry, delete that now
    if (sharedGeom)
    {
        parentComponent->removeChild(sharedGeom);
        vsObject::unrefDelete(sharedGeom);
    }
        
    // Dispose of the master component and texture
    vsObject::unrefDelete(masterTexture);
    vsObject::unrefDelete(parentComponent);
    
    // Delete the update timer
    delete updateTimer;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsParticleSystem::getClassName()
{
    return "vsParticleSystem";
}

// ------------------------------------------------------------------------
// Updates the particles in this system based on real-time.
// ------------------------------------------------------------------------
void vsParticleSystem::update()
{
   // Get the time elapsed since the last update.
   double frameTime = updateTimer->getElapsed();
   
   // Call the main update function with this value.
   update(frameTime);
}

// ------------------------------------------------------------------------
// Updates the particles in this system based on a delta-time value.
// ------------------------------------------------------------------------
void vsParticleSystem::update(double deltaTime)
{
    double frameTime;
    vsMatrix followNodeMatrix;
    double emitInterval;
    double nextEmitTime;
    vsParticle *particle;
    int loop;
    vsQuat rotQuat;

    // Get the amount of time to advance this frame
    frameTime = deltaTime;

    // Determine the amount of time between emitted particles
    emitInterval = (1.0 / emissionRate);
    
    // If we're using hardware shading, begin a new dynamic geometry
    // state
    if (hardwareShading)
        sharedGeom->beginNewState();

    // Get the current position and orientation of the follow node, if there
    // is one
    if (emitterFollowNode)
    {
        followNodeMatrix = emitterFollowNode->getGlobalXform();
        followNodeMatrix.getTranslation(&(currentFollowNodePos[0]),
                                        &(currentFollowNodePos[1]),
                                        &(currentFollowNodePos[2]));
        currentFollowNodeOri.setMatrixRotation(followNodeMatrix);
    }
    else
    {
        currentFollowNodePos.set(0.0, 0.0, 0.0);
        currentFollowNodeOri.set(0.0, 0.0, 0.0, 1.0);
    }

    // Update all currently active particles
    for (loop = 0; loop < particleListSize; loop++)
    {
        particle = (vsParticle *)(particleList[loop]);
        if (particle->isActive)
            updateParticle(particle, frameTime);
    }

    // Determine if the emitter is still emitting
    if (emitterActive &&
        ((emitterAge <= emitterLifetime) || (emitterLifetime < 0.0)) )
    {
        // Determine the time during this frame that the first particle is
        // emitted, if any
        nextEmitTime = emitInterval - emissionTimer;
        if (nextEmitTime < 0.0)
            nextEmitTime = 0.0;

        // Keep emitting particles as long as we're still in this frame.
        // (Also make sure that we have particles left to emit.)
        while ((nextEmitTime <= frameTime) &&
            (activeParticleCount < particleListSize))
        {
            // Emit the particle
            particle = (vsParticle *)(particleList[nextInactiveParticleIdx]);
            activateParticle(particle, nextEmitTime, frameTime);

            // Update the 'next available particle' pointer
            findNextInactive();

            // Advance to the next emission time
            nextEmitTime += emitInterval;
        }
    }
    
    // If we're using hardware shading, signal that we're done changing
    // the geometry for this frame
    if (hardwareShading)
        sharedGeom->finishNewState();

    // Update the 'time spent waiting for next emission' variable. This
    // involves adding in the amount of time that passed this frame, and then
    // subtracting out the emission interval time for each interval that
    // passed.
    emissionTimer = fmod(emissionTimer + frameTime, emitInterval);

    // Apply the emitter's velocity and angular velocity to its position and
    // orientation
    emitterPosition.add(emitterVelocity.getScaled(frameTime));
    rotQuat.setAxisAngleRotation(emitterAngularVelocityAxis[0],
                                 emitterAngularVelocityAxis[1],
                                 emitterAngularVelocityAxis[2],
                                 emitterAngularVelocitySpeed * frameTime);
    emitterOrientation = rotQuat * emitterOrientation;

    // Age the emitter
    emitterAge += frameTime;

    // Copy the current follow node data to the 'previous' node data holders
    // for use next frame
    prevFollowNodePos = currentFollowNodePos;
    prevFollowNodeOri = currentFollowNodeOri;
    prevFollowDataValid = true;
    
    // Mark the update timer for the next update call.
    updateTimer->mark();
}

// ------------------------------------------------------------------------
// Gets the vsComponent with all of the particle's geometry attached
// ------------------------------------------------------------------------
vsComponent *vsParticleSystem::getComponent()
{
    return parentComponent;
}

// ------------------------------------------------------------------------
// Sets the render bin for all particle geometry to use.  Often, particle
// systems don't live well with other transparent geometry.  Setting the
// render bin can fix this.
// ------------------------------------------------------------------------
void vsParticleSystem::setRenderBin(int newBin)
{
    vsParticle *particle;
    int loop;

    // Change the bin number for all new particles
    particleRenderBin = newBin;
    
    // Set the render bin on the shared geometry node.
    sharedGeom->setRenderBin(particleRenderBin);

    // Set the render bin on all existing particles
    for (loop = 0; loop < particleListSize; loop++)
    {
        particle = (vsParticle *)(particleList[loop]);
        
        // Make sure this geometry is initialized first...
        if(particle->quadGeometry != NULL)
        {
            particle->quadGeometry->setRenderBin(particleRenderBin);
        }
    }
}

// ------------------------------------------------------------------------
// Removes all currently visible particles and restarts the particle
// generation sequence from the beginning
// ------------------------------------------------------------------------
void vsParticleSystem::reset()
{
    int loop;
    vsParticle *particle;

    // Deactivate all active particles
    for (loop = 0; loop < particleListSize; loop++)
    {
        particle = (vsParticle *)(particleList[loop]);
        if (particle->isActive)
            deactivateParticle(particle);
    }

    // Sanity check; there shouldn't be any active particles
    if (activeParticleCount != 0)
        printf("vsParticleSystem::reset: Active particle count is not zero "
            "after reset\n");

    // Reset the emitter age and force it to be active
    emitterAge = 0.0;
    emitterActive = true;
    emissionTimer = 0.0;

    // Reset the next inactive particle pointer
    nextInactiveParticleIdx = 0;
}

// ------------------------------------------------------------------------
// Pauses creation of new particles. Currently created particles are
// unaffected.
// ------------------------------------------------------------------------
void vsParticleSystem::pauseEmission()
{
    emitterActive = false;
}

// ------------------------------------------------------------------------
// Resumes creation of new particles
// ------------------------------------------------------------------------
void vsParticleSystem::resumeEmission()
{
    emitterActive = true;
}

// ------------------------------------------------------------------------
// Gets Emitter's "expiration status" (true if expired, false otherwise
// ------------------------------------------------------------------------
bool vsParticleSystem::isEmitterExpired()
{
    if((emitterAge <= emitterLifetime) || (emitterLifetime < 0.0))
    {
        return false;
    }
    else
    {
        return true;
    }
}

// ------------------------------------------------------------------------
// Sets the location of the center point of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterPosition(vsVector position)
{
    emitterPosition.clearCopy(position);
}

// ------------------------------------------------------------------------
// Gets the location of the center point of the particle emitter
// ------------------------------------------------------------------------
vsVector vsParticleSystem::getEmitterPosition()
{
    return emitterPosition;
}

// ------------------------------------------------------------------------
// Sets the velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterVelocity(vsVector velocity)
{
    emitterVelocity.clearCopy(velocity);
}

// ------------------------------------------------------------------------
// Gets the velocity of the particle emitter
// ------------------------------------------------------------------------
vsVector vsParticleSystem::getEmitterVelocity()
{
    return emitterVelocity;
}

// ------------------------------------------------------------------------
// Sets the orientation of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterOrientation(vsQuat orientation)
{
    emitterOrientation = orientation;
}

// ------------------------------------------------------------------------
// Gets the orientation of the particle emitter
// ------------------------------------------------------------------------
vsQuat vsParticleSystem::getEmitterOrientation()
{
    return emitterOrientation;
}

// ------------------------------------------------------------------------
// Sets the angular velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterAngularVelocity(vsVector rotationAxis,
    double degreesPerSecond)
{
    emitterAngularVelocityAxis.clearCopy(rotationAxis);
    emitterAngularVelocitySpeed = degreesPerSecond;
}

// ------------------------------------------------------------------------
// Gets the angular velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getEmitterAngularVelocity(vsVector *rotationAxis,
                                                 double *degreesPerSecond)
{
    if (rotationAxis)
        (*rotationAxis) = emitterAngularVelocityAxis;
    if (degreesPerSecond)
        *degreesPerSecond = emitterAngularVelocitySpeed;
}

// ------------------------------------------------------------------------
// Sets the vsComponent that the emitter should move along with, or NULL
// if the emitter should not follow a component. If this component is set,
// then the values of the emitter center point and orientation are relative
// to the coordinate system of the component rather than relative to the
// origin.
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterFollowComponent(vsComponent *component)
{
    emitterFollowNode = component;
}

// ------------------------------------------------------------------------
// Gets the vsComponent that the emitter should move along with, or NULL
// if the emitter should not follow a component. If this component is set,
// then the values of the emitter center point and orientation are relative
// to the coordinate system of the component rather than relative to the
// origin.
// ------------------------------------------------------------------------
vsComponent *vsParticleSystem::getEmitterFollowComponent()
{
    return emitterFollowNode;
}

// ------------------------------------------------------------------------
// Sets the time, in seconds, that the emitter continues to create
// particles for. If the lifetime is negative, then the emitter continues
// to operate indefinitely.
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterLifetime(double seconds)
{
    emitterLifetime = seconds;
}

// ------------------------------------------------------------------------
// Gets the time, in seconds, that the emitter continues to create
// particles for. If the lifetime is negative, then the emitter continues
// to operate indefinitely.
// ------------------------------------------------------------------------
double vsParticleSystem::getEmitterLifetime()
{
    return emitterLifetime;
}

// ------------------------------------------------------------------------
// Sets the speed, in particles per second, at which the emitter creates
// new particles
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterRate(double particlesPerSecond)
{
    emissionRate = particlesPerSecond;
}

// ------------------------------------------------------------------------
// Gets the speed, in particles per second, at which the emitter creates
// new particles
// ------------------------------------------------------------------------
double vsParticleSystem::getEmitterRate()
{
    return emissionRate;
}

// ------------------------------------------------------------------------
// Sets the shape of the emitter region. The shape parameter determines
// the actual shape of the region. The minRadius and maxRadius parameters
// describe the inner and outer radii of the region (for creating 'rings'
// and 'hollow' regions).
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterShape(vsParticleSystemEmitterShape shape,
    double minRadius, double maxRadius)
{
    // Maximum radius must be at least as large as minimum radius
    if (minRadius > maxRadius)
    {
        printf("vsParticleSystem::setEmitterShape: Maximum radius must be "
            "larger than minimum radius\n");
        return;
    }

    // Both radii must be nonnegative
    if (minRadius < 0.0)
    {
        printf("vsParticleSystem::setEmitterShape: Minimum radius must be "
            "nonnegative\n");
        return;
    }
    if (maxRadius < 0.0)
    {
        printf("vsParticleSystem::setEmitterShape: Maximum radius must be "
            "nonnegative\n");
        return;
    }

    // Copy the parameters
    emitterShape = shape;
    emitterMinRadius = minRadius;
    emitterMaxRadius = maxRadius;
}

// ------------------------------------------------------------------------
// Gets the shape of the emitter region. The shape parameter determines
// the actual shape of the region. The minRadius and maxRadius parameters
// describe the inner and outer radii of the region (for creating 'rings'
// and 'hollow' regions).
// ------------------------------------------------------------------------
void vsParticleSystem::getEmitterShape(vsParticleSystemEmitterShape *shape,
    double *minRadius, double *maxRadius)
{
    if (shape)
        *shape = emitterShape;
    if (minRadius)
        *minRadius = emitterMinRadius;
    if (maxRadius)
        *maxRadius = emitterMaxRadius;
}

// ------------------------------------------------------------------------
// Sets the maximum number of active particles in this system. Calls the
// reset() method as a side effect.
// ------------------------------------------------------------------------
void vsParticleSystem::setMaxParticleCount(int maxParticles)
{
    int oldSize;
    int loop;

    // Sanity check; must be at least one particle
    if (maxParticles < 1)
    {
        printf("vsParticleSystem::setMaxParticleCount: Invalid particle "
            "count\n");
        return;
    }

    // Resize the particle list
    if (maxParticles > particleListSize)
    {
        particleList.setSize(maxParticles);

        if (hardwareShading)
        {
            sharedGeom->beginNewState();
            sharedGeom->setPrimitiveCount(maxParticles);
            sharedGeom->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_NORMALS, maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_COLORS, maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE0_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE1_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE2_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE3_COORDS, 
                maxParticles*4);

            primInUse.setSize(maxParticles);
        }

        oldSize = particleListSize;
        particleListSize = maxParticles;

        // The list is growing; create new particle structures for the new
        // list entries
        for (loop = oldSize; loop < particleListSize; loop++)
            particleList[loop] = createParticle();
            
        if (hardwareShading)
            sharedGeom->finishNewState();
    }
    else if (maxParticles < particleListSize)
    {
        if (hardwareShading)
            sharedGeom->beginNewState();
            
        // The list is shrinking; destroy the particle structures for the
        // disappearing list entries
        for (loop = maxParticles; loop < particleListSize; loop++)
            destroyParticle((vsParticle *)(particleList[loop]));

        particleList.setSize(maxParticles);
        particleListSize = maxParticles;

        if (hardwareShading)
        {
            sharedGeom->setPrimitiveCount(maxParticles);
            sharedGeom->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_NORMALS, maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_COLORS, maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE0_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE1_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE2_COORDS, 
                maxParticles*4);
            sharedGeom->setDataListSize(VS_GEOMETRY_TEXTURE3_COORDS, 
                maxParticles*4);
            sharedGeom->finishNewState();

            primInUse.setSize(maxParticles);
        }
    }

    // Since the list size changed, a lot of our instance variables could
    // be wrong; do a reset call to force everything back into place.
    reset();
}

// ------------------------------------------------------------------------
// Gets the maximum number of active particles in this system
// ------------------------------------------------------------------------
int vsParticleSystem::getMaxParticleCount()
{
    return particleListSize;
}

// ------------------------------------------------------------------------
// Sets the texture to use for the particles by its filename
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleTexture(char *textureFilename)
{
    // Sanity check
    if (!textureFilename)
    {
        printf("vsParticleSystem::setParticleTexture: Invalid filename "
            "(NULL)\n");
        return;
    }

    // Pass the filename to the particles' texture attribute
    masterTexture->loadImageFromFile(textureFilename);
}

// ------------------------------------------------------------------------
// Sets the constant acceleration applied to all particles
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleAcceleration(vsVector acceleration)
{
    globalAcceleration.clearCopy(acceleration);
}

// ------------------------------------------------------------------------
// Gets the constant acceleration applied to all particles
// ------------------------------------------------------------------------
vsVector vsParticleSystem::getParticleAcceleration()
{
    return globalAcceleration;
}

// ------------------------------------------------------------------------
// Sets the time (and variance) in seconds for which each particle is
// active
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleLifetime(double seconds, double variance)
{
    lifetime = seconds;
    lifetimeVariance = variance;
}

// ------------------------------------------------------------------------
// Gets the time (and variance) in seconds for which each particle is
// active
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleLifetime(double *seconds, 
                                                 double *variance)
{
    if (seconds)
        *seconds = lifetime;
    if (variance)
        *variance = lifetimeVariance;
}

// ------------------------------------------------------------------------
// Sets the initial velocity and variance parameters for the individual
// particles. The velocity parameter is used as the initial velocity.
// The minAngleVariance and maxAngleVariance parameters specify minimum
// and maximum degree measures by which each particles velocity differs
// from the specified velocity. The speedVariance parameter specifies the
// maximum difference of the magnitude of the velocity from the specified
// velocity.
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleVelocity(vsVector velocity,
    double minAngleVariance, double maxAngleVariance, double speedVariance)
{
    // Sanity check; maximum angle must be at least as large as minimum
    if (minAngleVariance > maxAngleVariance)
    {
        printf("vsParticleSystem::setParticleVelocity: Maximum angle "
            "variance must be larger than minimum angle variance\n");
        return;
    }

    initialVelocity.clearCopy(velocity);
    velocityMinAngleVariance = minAngleVariance;
    velocityMaxAngleVariance = maxAngleVariance;
    velocitySpeedVariance = speedVariance;
}

// ------------------------------------------------------------------------
// Gets the initial velocity and variance parameters for the individual
// particles. The velocity parameter is used as the initial velocity.
// The minAngleVariance and maxAngleVariance parameters specify minimum
// and maximum degree measures by which each particles velocity differs
// from the specified velocity. The speedVariance parameter specifies the
// maximum difference of the magnitude of the velocity from the specified
// velocity.
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleVelocity(vsVector *velocity,
    double *minAngleVariance, double *maxAngleVariance, double *speedVariance)
{
    if (velocity)
        (*velocity) = initialVelocity;
    if (minAngleVariance)
        *minAngleVariance = velocityMinAngleVariance;
    if (maxAngleVariance)
        *maxAngleVariance = velocityMaxAngleVariance;
    if (speedVariance)
        *speedVariance = velocitySpeedVariance;
}

// ------------------------------------------------------------------------
// Sets the speed (and variance), in degrees per second, at which the
// particle revolves around the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleOrbitSpeed(double speed, double variance)
{
    orbitSpeed = speed;
    orbitSpeedVariance = variance;
}

// ------------------------------------------------------------------------
// Gets the speed (and variance), in degrees per second, at which the
// particle revolves around the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleOrbitSpeed(double *speed,
                                                   double *variance)
{
    if (speed)
        *speed = orbitSpeed;
    if (variance)
        *variance = orbitSpeedVariance;
}

// ------------------------------------------------------------------------
// Sets the speed (and variance) at which the particle moves away from (or
// towards, for negative values) the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleOrbitRadiusDelta(double speed,
    double variance)
{
    orbitRadiusDelta = speed;
    orbitRadiusDeltaVariance = variance;
}

// ------------------------------------------------------------------------
// Gets the speed (and variance) at which the particle moves away from (or
// towards, for negative values) the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleOrbitRadiusDelta(double *speed,
    double *variance)
{
    if (speed)
        *speed = orbitRadiusDelta;
    if (variance)
        *variance = orbitRadiusDeltaVariance;
}

// ------------------------------------------------------------------------
// Sets the initial and final sizes (and variances) of the particles over
// their lifetimes. The uniform flag specifies if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances.
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleSize(double initial, double initialVariance,
    double final, double finalVariance, bool uniform)
{
    initialSize = initial;
    initialSizeVariance = initialVariance;
    finalSize = final;
    finalSizeVariance = finalVariance;
    lockSizeVariance = uniform;
}

// ------------------------------------------------------------------------
// Gets the initial and final sizes (and variances) of the particles over
// their lifetimes. The uniform flag specifies if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances.
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleSize(double *initial, 
    double *initialVariance, double *final, double *finalVariance,
    bool *uniform)
{
    if (initial)
        *initial = initialSize;
    if (initialVariance)
        *initialVariance = initialSizeVariance;
    if (final)
        *final = finalSize;
    if (finalVariance)
        *finalVariance = finalSizeVariance;
    if (uniform)
        *uniform = lockSizeVariance;
}

// ------------------------------------------------------------------------
// Sets the rotation, in degrees, of each particle around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleRotation(double rotationDegrees,
    double variance)
{
    rotation = rotationDegrees;
    rotationVariance = variance;
}

// ------------------------------------------------------------------------
// Gets the rotation, in degrees, of each particle around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleRotation(double *rotationDegrees,
    double *variance)
{
    if (rotationDegrees)
        *rotationDegrees = rotation;
    if (variance)
        *variance = rotationVariance;
}

// ------------------------------------------------------------------------
// Sets the speed, in degrees per second, of the rotation of each particle
// around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleRotationSpeed(double degreesPerSecond,
    double variance)
{
    rotationSpeed = degreesPerSecond;
    rotationSpeedVariance = variance;
}

// ------------------------------------------------------------------------
// Gets the speed, in degrees per second, of the rotation of each particle
// around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleRotationSpeed(double *degreesPerSecond,
    double *variance)
{
    if (degreesPerSecond)
        *degreesPerSecond = rotationSpeed;
    if (variance)
        *variance = rotationSpeedVariance;
}

// ------------------------------------------------------------------------
// Sets the initial and final colors (and variances) of the particles over
// their lifetimes. The uniform flags specify if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances. The uniformIntra flag is for forcing variance
// fractions within the color to be the same, while the uniformInter flag
// does the same for variance fractions between the two colors.
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleColor(vsVector initial,
    vsVector initialVariance, vsVector final, vsVector finalVariance,
    bool uniformIntra, bool uniformInter)
{
    // If no alpha specified, force it to be 1
    initialColor.clearCopy(initial);
    if (initial.getSize() < 4)
        initialColor[3] = 1.0;

    initialColorVariance.clearCopy(initialVariance);
    
    // If no alpha specified, force it to be 1
    finalColor.clearCopy(final);
    if (final.getSize() < 4)
        finalColor[3] = 1.0;

    finalColorVariance.clearCopy(finalVariance);

    lockIntraColorVariance = uniformIntra;
    lockInterColorVariance = uniformInter;
}

// ------------------------------------------------------------------------
// Gets the initial and final colors (and variances) of the particles over
// their lifetimes. The uniform flags specify if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances. The uniformIntra flag is for forcing variance
// fractions within the color to be the same, while the uniformInter flag
// does the same for variance fractions between the two colors.
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleColor(vsVector *initial,
    vsVector *initialVariance, vsVector *final, vsVector *finalVariance,
    bool *uniformIntra, bool *uniformInter)
{
    if (initial)
        (*initial) = initialColor;
    if (initialVariance)
        (*initialVariance) = initialColorVariance;
    if (final)
        (*final) = finalColor;
    if (finalVariance)
        (*finalVariance) = finalColorVariance;
    if (uniformIntra)
        *uniformIntra = lockIntraColorVariance;
    if (uniformInter)
        *uniformInter = lockInterColorVariance;
}

// ------------------------------------------------------------------------
// Private function
// Constructs a vsParticle structure and sets it up its components
// ------------------------------------------------------------------------
vsParticle *vsParticleSystem::createParticle()
{
    vsParticle *result;
    vsComponent *translationComponent, *billboardComponent;
    vsComponent *rotScaleComponent;
    vsGeometry *geometry;
    vsTransformAttribute *translateAttr, *rotScaleAttr;
    vsBillboardAttribute *bbAttr;
    int primIndex;

    // Create the particle structure
    result = new vsParticle;

    // Set up the various transforms if we're using software mode
    if (!hardwareShading)
    {
        // Create the component with the particle position transform
        translationComponent = new vsComponent();
        translateAttr = new vsTransformAttribute();
        translationComponent->addAttribute(translateAttr);

        // Create the component that houses the billboard attribute
        billboardComponent = new vsComponent();
        bbAttr = new vsBillboardAttribute();
        bbAttr->setMode(VS_BILLBOARD_ROT_POINT_EYE);
        bbAttr->setFrontDirection(vsVector(0.0, 0.0, 1.0));
        bbAttr->setAxis(vsVector(0.0, 1.0, 0.0));
        billboardComponent->addAttribute(bbAttr);

        // Create the component that houses the rotation & scaling matrix
        rotScaleComponent = new vsComponent();
        rotScaleAttr = new vsTransformAttribute();
        rotScaleComponent->addAttribute(rotScaleAttr);

        // Create the particle's geometry
        geometry = new vsGeometry();
        geometry->setRenderBin(particleRenderBin);
        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
        geometry->setPrimitiveCount(1);

        geometry->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 4);
        geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 0,
            vsVector(-0.5, -0.5, 0.0));
        geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 1,
            vsVector( 0.5, -0.5, 0.0));
        geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 2,
            vsVector( 0.5,  0.5, 0.0));
        geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 3,
            vsVector(-0.5,  0.5, 0.0));

        geometry->setBinding(VS_GEOMETRY_NORMALS, VS_GEOMETRY_BIND_OVERALL);
        geometry->setDataListSize(VS_GEOMETRY_NORMALS, 1);
        geometry->setData(VS_GEOMETRY_NORMALS, 0, vsVector(0.0, 0.0, 1.0));

        geometry->setBinding(VS_GEOMETRY_COLORS, VS_GEOMETRY_BIND_OVERALL);
        geometry->setDataListSize(VS_GEOMETRY_COLORS, 1);
        geometry->setData(VS_GEOMETRY_COLORS, 0, vsVector(1.0, 1.0, 1.0, 1.0));

        geometry->setBinding(VS_GEOMETRY_TEXTURE_COORDS,
            VS_GEOMETRY_BIND_PER_VERTEX);
        geometry->setDataListSize(VS_GEOMETRY_TEXTURE_COORDS, 4);
        geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 0, vsVector(0.0, 0.0));
        geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 1, vsVector(1.0, 0.0));
        geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 2, vsVector(1.0, 1.0));
        geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 3, vsVector(0.0, 1.0));

        geometry->enableLighting();

        geometry->setIntersectValue(0x00000001);

        // Connect the chain of nodes together
        translationComponent->addChild(billboardComponent);
        billboardComponent->addChild(rotScaleComponent);
        rotScaleComponent->addChild(geometry);

        // Keep the objects that we'll need later
        result->mainComponent = translationComponent;
        result->mainComponent->ref();
        result->positionAttr = translateAttr;
        result->positionAttr->ref();
        result->rotScaleAttr = rotScaleAttr;
        result->rotScaleAttr->ref();
        result->quadGeometry = geometry;
        result->quadGeometry->ref();
    }
    else
    {
        // Figure out this particle's primitive index
        primIndex = 0;
        while ((primIndex < particleListSize) && 
            (primInUse[primIndex]))
            primIndex++;

        if (primIndex >= particleListSize)
        {
            printf("vsParticleSystem::createParticle:  Particle list is "
                "full!\n");
            return NULL;
        }

        // Mark this particle as in use in the shared geometry
        primInUse[primIndex] = (void *)true;

        // Multiply the primitive index by 4 to get the actual vertex index 
        // in the shared geometry node
        result->geomIndex = primIndex * 4;

        // Set up the particle's position, normal, color and texture coords
        sharedGeom->setData(VS_GEOMETRY_VERTEX_COORDS, result->geomIndex + 0,
            vsVector(-0.5, -0.5, 0.0));
        sharedGeom->setData(VS_GEOMETRY_VERTEX_COORDS, result->geomIndex + 1,
            vsVector( 0.5, -0.5, 0.0));
        sharedGeom->setData(VS_GEOMETRY_VERTEX_COORDS, result->geomIndex + 2,
            vsVector( 0.5,  0.5, 0.0));
        sharedGeom->setData(VS_GEOMETRY_VERTEX_COORDS, result->geomIndex + 3,
            vsVector(-0.5,  0.5, 0.0));
        sharedGeom->setData(VS_GEOMETRY_NORMALS, result->geomIndex + 0,
            vsVector(0.0, 0.0, 1.0));
        sharedGeom->setData(VS_GEOMETRY_NORMALS, result->geomIndex + 1,
            vsVector(0.0, 0.0, 1.0));
        sharedGeom->setData(VS_GEOMETRY_NORMALS, result->geomIndex + 2,
            vsVector(0.0, 0.0, 1.0));
        sharedGeom->setData(VS_GEOMETRY_NORMALS, result->geomIndex + 3,
            vsVector(0.0, 0.0, 1.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, result->geomIndex + 0,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, result->geomIndex + 1,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, result->geomIndex + 2,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, result->geomIndex + 3,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE0_COORDS, result->geomIndex + 0,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE0_COORDS, result->geomIndex + 1,
            vsVector(1.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE0_COORDS, result->geomIndex + 2,
            vsVector(1.0, 1.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE0_COORDS, result->geomIndex + 3,
            vsVector(0.0, 1.0));

        // Texture coordinate 1 holds the particle's X and Y position
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS, result->geomIndex + 0,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS, result->geomIndex + 1,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS, result->geomIndex + 2,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS, result->geomIndex + 3,
            vsVector(0.0, 0.0));

        // Texture coordinate 2 holds the particle's Z position and rotation
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS, result->geomIndex + 0,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS, result->geomIndex + 1,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS, result->geomIndex + 2,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS, result->geomIndex + 3,
            vsVector(0.0, 0.0));

        // Texture coordinate 3 holds the particle's size
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS, result->geomIndex + 0,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS, result->geomIndex + 1,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS, result->geomIndex + 2,
            vsVector(0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS, result->geomIndex + 3,
            vsVector(0.0, 0.0));

        // Set the remaining structure members to NULL (we don't use them
        // in hardware mode
        result->mainComponent = NULL;
        result->quadGeometry = NULL;
        result->positionAttr = NULL;
        result->rotScaleAttr = NULL;
    }

    // Set the particle to inactive
    result->isActive = false;

    // We shouldn't need to set any of the other particle fields, as they will
    // get set when the particle is activated. We're done.
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Destroys an inactive vsParticle structure. If the particle is active,
// calls deactivateParticle on it first.
// ------------------------------------------------------------------------
void vsParticleSystem::destroyParticle(vsParticle *particle)
{
    int primIndex;

    // Deactivate the particle first, if needed
    if (particle->isActive)
        deactivateParticle(particle);

    // Dispose of the nodes and attributes
    if (hardwareShading)
    {
        primIndex = particle->geomIndex / 4;
        primInUse[primIndex] = (void *)false;
    }
    else
    {
        vsObject::unrefDelete(particle->mainComponent);
        vsObject::unrefDelete(particle->positionAttr);
        vsObject::unrefDelete(particle->rotScaleAttr);
        vsObject::unrefDelete(particle->quadGeometry);
    }

    // Destroy the particle structure
    delete particle;
}

// ------------------------------------------------------------------------
// Private function
// Activates an inactive particle by determining the particle's attributes
// (velocity, color, etc.), adding its geometry to the particle system
// master component, and registering the particle as active.
// ------------------------------------------------------------------------
void vsParticleSystem::activateParticle(vsParticle *particle,
    double creationDelay, double frameTime)
{
    // This function attempts to take into account the exact time during the
    // last frame that the particle was supposed to be created, and creates
    // the particle so that it is in sync for the end of this frame. This is
    // done in two parts: The first is to determine how far into the frame the
    // particle should be created, and interpolate details such as emitter
    // position and orientation between those values from last frame and the
    // ones at the end of this frame. The second part is a call to the
    // particle update function with the amount of time in the remainder of
    // the frame, this gets the particle into 'temporal sync' with the rest of
    // the active particles.

    vsMatrix emitterMat, objectMat, totalMat;
    vsMatrix posMat, oriMat;
    vsVector pos;
    vsQuat ori, ori2;
    double frameRatio;
    double distance, headingDegs, pitchDegs;
    vsVector direction;
    vsVector xDir, yDir, zDir;
    double speed;
    double variance;
    int loop;
    double variances[4];
    double min, max;

    // Don't activate an active particle
    if (particle->isActive)
        return;

    // Attach the particle's geometry to the particle system's master
    // component
    if (!hardwareShading)
        parentComponent->addChild(particle->mainComponent);

    // Determine the particle's lifetime and set it to zero age
    particle->lifetimeSeconds = lifetime +
        (lifetimeVariance * getRandomVariance());
    particle->ageSeconds = 0.0;
    // The lifetime must be a positive number; otherwise, no one will ever
    // see the particle.
    if (particle->lifetimeSeconds < VS_DEFAULT_TOLERANCE)
        particle->lifetimeSeconds = VS_DEFAULT_TOLERANCE;

    // * Calculate the total transformation matrix from the origin to the
    // emitter coordinate system, taking both the follow node transform and
    // emitter-specific transform into account

    // If the position and orientation of the follow node during the previous
    // update are known, then interpolate the position and orientation of the
    // follow node at the time the particle is created; otherwise, just use
    // the follow node's current position and orientation.
    if (prevFollowDataValid)
    {
        frameRatio = (creationDelay / frameTime);
        pos = prevFollowNodePos.getScaled(1.0 - frameRatio) +
            currentFollowNodePos.getScaled(frameRatio);
        ori = prevFollowNodeOri.slerp(currentFollowNodeOri, frameRatio);
    }
    else
    {
        pos = currentFollowNodePos;
        ori = currentFollowNodeOri;
    }
    
    // Create a matrix from the position and orientation
    posMat.setTranslation(pos[0], pos[1], pos[2]);
    oriMat.setQuatRotation(ori);
    objectMat = posMat * oriMat;

    // Extrapolate the position and orientation of the emitter at the time
    // the particle is created
    pos = emitterPosition + emitterVelocity.getScaled(creationDelay);
    // Compute the amount of orientation change
    ori2.setAxisAngleRotation(emitterAngularVelocityAxis[0],
                              emitterAngularVelocityAxis[1],
                              emitterAngularVelocityAxis[2],
                              emitterAngularVelocitySpeed * creationDelay);
    // Compute the total orientation
    ori = ori2 * emitterOrientation;
    // Create a matrix from the position and orientation
    posMat.setTranslation(pos[0], pos[1], pos[2]);
    oriMat.setQuatRotation(ori);
    emitterMat = posMat * oriMat;

    // Compute the final emission matrix as the composite of the emitter local
    // coordinate matrix and the node object space coordinate matrix
    totalMat = objectMat * emitterMat;
    particle->emitterMatrix = totalMat;

    // * Randomly compute the particle's initial position based on the shape,
    // orientation, and radii of the emitter

    // Then account for the emitter's shape
    switch (emitterShape)
    {
        case VS_PARTICLESYS_EMITTER_POINT:
            // All particles come from the emitter center point
            particle->position.set(0.0, 0.0, 0.0);
            break;

        case VS_PARTICLESYS_EMITTER_LINE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Randomly determine 'forward' or 'back'
            if (getRandomVariance() > 0.0)
            {
                // forward
                particle->position.set(0.0, distance, 0.0);
            }
            else
            {
                // back
                particle->position.set(0.0, -distance, 0.0);
            }
            break;

        case VS_PARTICLESYS_EMITTER_CIRCLE:
            // Randomly determine the distance from the emitter center
            // * This calculation includes a 'bias' to create more points
            // farther away from the circle's center; this is to prevent the
            // tendency of evenly-distributed points to 'bunch up' near the
            // center of the circle. The bias in implemented by taking the
            // square root of a number in the range [0,1] before scaling it
            // into the radius.
            if (emitterMaxRadius > 0.0)
            {
                min = VS_SQR(emitterMinRadius / emitterMaxRadius);
                distance = min + ((1.0 - min) * getRandom());
                distance = sqrt(distance);
                distance *= emitterMaxRadius;
            }
            else
            {
                // Avoid dividing by zero; just assume a zero distance
                distance = 0.0;
            }

            // Randomly determine a heading
            headingDegs = 360.0 * getRandom();

            // Compute the position from the heading
            particle->position.set(
                distance * cos(VS_DEG2RAD(headingDegs)),
                distance * sin(VS_DEG2RAD(headingDegs)),
                0.0);

            break;

        case VS_PARTICLESYS_EMITTER_SPHERE:
            // Randomly determine the distance from the emitter center
            // * This calculation includes a 'bias' to create more points
            // farther away from the sphere's center; this is to prevent the
            // tendency of evenly-distributed points to 'bunch up' near the
            // center of the sphere. The bias in implemented by taking the
            // cube root of a number in the range [0,1] before scaling it
            // into the radius.
            if (emitterMaxRadius > 0.0)
            {
                min = VS_SQR(emitterMinRadius / emitterMaxRadius);
                distance = min + ((1.0 - min) * getRandom());
                distance = pow(distance, 1.0/3.0);
                distance *= emitterMaxRadius;
            }
            else
            {
                // Avoid dividing by zero; just assume a zero distance
                distance = 0.0;
            }

            // Randomly determine a heading and pitch
            headingDegs = 360.0 * getRandom();
            // Compute the pitch like this, instead of just any old random
            // angle between -90 and 90, to prevent the points from 'bunching
            // up' at the sphere's poles.
            pitchDegs = VS_RAD2DEG(asin(getRandomVariance()));

            // Compute the position from the heading and pitch
            particle->position.set(
                cos(VS_DEG2RAD(headingDegs)) * cos(VS_DEG2RAD(pitchDegs)),
                sin(VS_DEG2RAD(headingDegs)) * cos(VS_DEG2RAD(pitchDegs)),
                sin(VS_DEG2RAD(pitchDegs)));
            particle->position.scale(distance);

            break;

        case VS_PARTICLESYS_EMITTER_SQUARE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Compute random values for the two variable coordinates
            particle->position.set(
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance(),
                0.0);

            // Force one (random) coordinate into the min/max radius
            // constraints
            switch ((int)(floor(getRandom() * 4.0)))
            {
                case 0:
                    // Positive X
                    particle->position[0] = distance;
                    break;

                case 1:
                    // Negative X
                    particle->position[0] = -distance;
                    break;

                case 2:
                    // Positive Y
                    particle->position[1] = distance;
                    break;

                default:
                    // Negative Y
                    particle->position[1] = -distance;
                    break;
            }

            break;

        case VS_PARTICLESYS_EMITTER_CUBE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Compute random values for the three variable coordinates
            particle->position.set(
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance());

            // Force one (random) coordinate into the min/max radius
            // constraints
            switch ((int)(floor(getRandom() * 6.0)))
            {
                case 0:
                    // Positive X
                    particle->position[0] = distance;
                    break;

                case 1:
                    // Negative X
                    particle->position[0] = -distance;
                    break;

                case 2:
                    // Positive Y
                    particle->position[1] = distance;
                    break;

                case 3:
                    // Negative Y
                    particle->position[1] = -distance;
                    break;

                case 4:
                    // Positive Z
                    particle->position[2] = distance;
                    break;

                default:
                    // Negative Z
                    particle->position[2] = -distance;
                    break;
            }

            break;
    }

    // * Randomly compute the particle's velocity, based on the given angle
    // and speed variances
    direction = initialVelocity.getNormalized();
    speed = initialVelocity.getMagnitude();
    // Check for no initial velocity
    if (speed < VS_DEFAULT_TOLERANCE)
        particle->velocity.set(0.0, 0.0, 0.0);
    else
    {
        // * Compute two vectors orthogonal to the original direction vector
        // and to each other

        // Create any unit vector and force it to be orthogonal
        xDir.set(1.0, 0.0, 0.0);
        xDir -= direction.getScaled(direction.getDotProduct(xDir));
        if (xDir.getMagnitude() < VS_DEFAULT_TOLERANCE)
        {
            // Somehow we managed to pick a vector that's the same as the
            // original direction vector. Pick another one.
            xDir.set(0.0, 1.0, 0.0);
            xDir -= direction.getScaled(direction.getDotProduct(xDir));
        }
        xDir.normalize();

        // Create a second orthogonal vector by taking the cross product of
        // the two vectors we have now
        yDir = direction.getCrossProduct(xDir);
            

        // Compute a random angle variance and variance heading direction
        headingDegs = 360.0 * getRandom();
        // The pitch determination is complicated by the desire to evenly
        // distribute the directions around the arc area. A simple linear
        // random angle determination will cause the directions to 'bunch up'
        // around the 0-degree center direction. A bit of trig is required to
        // make the distribution a little more even.
        min = cos(VS_DEG2RAD(velocityMinAngleVariance));
        max = cos(VS_DEG2RAD(velocityMaxAngleVariance));
        pitchDegs = min + ((max - min) * getRandom());
        pitchDegs = VS_RAD2DEG(acos(pitchDegs));

        // Using the original direction as a Z axis, and our two new
        // directions as X and Y axes, construct a new direction using the
        // randomly-computed heading and pitch
        zDir = direction;
        direction =
            xDir.getScaled(cos(VS_DEG2RAD(headingDegs)) *
                sin(VS_DEG2RAD(pitchDegs))) +
            yDir.getScaled(sin(VS_DEG2RAD(headingDegs)) *
                sin(VS_DEG2RAD(pitchDegs))) +
            zDir.getScaled(cos(VS_DEG2RAD(pitchDegs)));

        // Factor the speed back in, modified by the speed variance
        direction.scale(speed +
            (velocitySpeedVariance * getRandomVariance()));

        // Set the result as the particle's velocity
        particle->velocity = direction;
    }

    // * Compute the orbit parameters

    // Calculate the initial orbit radius by calculating the distance from
    // the particle and the origin, after the particle is projected onto the
    // XY plane
    pos = particle->position;
    pos[2] = 0.0;
    particle->orbitRadius = pos.getMagnitude();

    // Calculate the initial orbit angle by taking the initial position of
    // the particle and checking its angle with the X axis when projected
    // into the XY plane.
    // Error checking: If the orbit radius is zero, then the orbit angle
    // calculation probably won't come out right. (atan2 tends to spit out
    // NAN values when given two zeroes.) Set the orbit angle to a default
    // value when this occurs.
    if (particle->orbitRadius < VS_DEFAULT_TOLERANCE)
        particle->orbitAngle = 0.0;
    else
        particle->orbitAngle = VS_RAD2DEG(atan2(pos[1], pos[0]));

    // Randomly compute the orbit velocity and orbit radius delta velocity
    particle->orbitVelocity = orbitSpeed +
        (orbitSpeedVariance * getRandomVariance());
    particle->orbitRadiusDelta = orbitRadiusDelta +
        (orbitRadiusDeltaVariance * getRandomVariance());

    // Since the particle's x and y-coordinate information is now effectively
    // stored in the orbit data, remove that same information from the
    // 'current position' field, as otherwise when we go to reconstruct the
    // particle position we'll be adding in that data twice.
    particle->position[0] = 0.0;
    particle->position[1] = 0.0;

    // * Randomly compute the particle's sizes over its lifetime
    variance = getRandomVariance();
    particle->initialSize = initialSize + (initialSizeVariance * variance);
    if (particle->initialSize < 0.0)
        particle->initialSize = 0.0;
    // Only compute a new variance if the size variance isn't uniform
    if (!lockSizeVariance)
        variance = getRandomVariance();
    particle->finalSize = finalSize + (finalSizeVariance * variance);
    if (particle->finalSize < 0.0)
        particle->finalSize = 0.0;

    // * Randomly compute the particle's rotation
    particle->rotation = rotation + (rotationVariance * getRandomVariance());
    particle->rotationSpeed = rotationSpeed +
        (rotationSpeedVariance * getRandomVariance());

    // * Randomly compute the particle's colors over its lifetime

    // Create an overall variance for the initial color, in case we're using
    // the intra-color lock mode
    variance = getRandomVariance();

    // Compute the initial color
    for (loop = 0; loop < 4; loop++)
    {
        if (lockIntraColorVariance)
        {
            // Use the same variance for each component of this color
            particle->initialColor[loop] = initialColor[loop] +
                (initialColorVariance[loop] * variance);
        }
        else
        {
            // Use a new variance for each component of this color, but store
            // the variances in case we need them for the final color
            variances[loop] = getRandomVariance();
            particle->initialColor[loop] = initialColor[loop] +
                (initialColorVariance[loop] * variances[loop]);
        }
    }

    // If there is no lock between the two colors, then get a new overall
    // variance for the final color
    if (!lockInterColorVariance)
        variance = getRandomVariance();

    // Compute the final color
    for (loop = 0; loop < 4; loop++)
    {
        if (lockIntraColorVariance)
        {
            // Use the same variance for each component of this color
            particle->finalColor[loop] = finalColor[loop] +
                (finalColorVariance[loop] * variance);
        }
        else
        {
            // Use a separate variance for each component of this color. If
            // we're in inter-color lock mode, use the variances from the
            // initial color; otherwise, make new ones.
            if (lockInterColorVariance)
                particle->finalColor[loop] = finalColor[loop] +
                    (finalColorVariance[loop] * variances[loop]);
            else
                particle->finalColor[loop] = finalColor[loop] +
                    (finalColorVariance[loop] * getRandomVariance());
        }
    }

    // * Use the particle update function to advance the particle in time to
    // be in sync with the rest of the active particles
    updateParticle(particle, frameTime - creationDelay);

    // Mark the particle as activated
    particle->isActive = true;
    activeParticleCount++;
}

// ------------------------------------------------------------------------
// Private function
// Deactivates an active particle by removing its geometry from the
// particle system master component, and registering the particle as
// inactive.
// ------------------------------------------------------------------------
void vsParticleSystem::deactivateParticle(vsParticle *particle)
{
    // Don't deactivate an inactive particle
    if (!(particle->isActive))
        return;

    // Remove the particle node chain from the system's master component
    if (hardwareShading)
    {
        // Hide the particle in the primitive list, by setting it's alpha
        // value to zero
        sharedGeom->setData(VS_GEOMETRY_COLORS, particle->geomIndex + 0,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, particle->geomIndex + 1,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, particle->geomIndex + 2,
            vsVector(0.0, 0.0, 0.0, 0.0));
        sharedGeom->setData(VS_GEOMETRY_COLORS, particle->geomIndex + 3,
            vsVector(0.0, 0.0, 0.0, 0.0));
    }
    else
    {
        // When using software, we attach a chain of three nodes rooted
        // at "mainComponent".  Detach this chain to deactivate the
        // particle.
        parentComponent->removeChild(particle->mainComponent);
    }

    // Mark this particle as inactive
    particle->isActive = false;

    // Note that there is one less active particle
    activeParticleCount--;

    // If this particle is the _only_ inactive particle, then make sure that
    // the next inactive particle pointer is pointing to it
    findNextInactive();
}

// ------------------------------------------------------------------------
// Private function
// Advances the indicated particle in time by the specified amount
// ------------------------------------------------------------------------
void vsParticleSystem::updateParticle(vsParticle *particle, double deltaTime)
{
    vsVector acceleration;
    vsVector position;
    vsMatrix posMat, rotMat, scaleMat;
    double currentSize;
    vsVector color;
    double lifeRatio;

    // * Update the various data fields of the particle

    // Increase the age of the particle. If this causes the particle to reach
    // the end of its lifespan, then remove it and return.
    particle->ageSeconds += deltaTime;
    if (particle->ageSeconds > particle->lifetimeSeconds)
    {
        deactivateParticle(particle);
        return;
    }

    // Compute the fraction of its lifetime that the particle has gone
    // through; we'll need this for interpolation later
    lifeRatio = (particle->ageSeconds / particle->lifetimeSeconds);

    // Compute the acceleration vector, taking into account the coordinate
    // system of the particle. (The particle's stored position is always in
    // its own coordinate system, which is the coordinate system of the
    // emitter at the point in time when the particle was created, while the
    // acceleration is in global coordinates.)
    acceleration = particle->emitterMatrix.getInverse().
        getVectorXform(globalAcceleration);

    // Compute the new position of the particle, using both the particle's
    // current velocity and the constant acceleration
    particle->position += (particle->velocity.getScaled(deltaTime) +
        acceleration.getScaled(VS_SQR(deltaTime)));

    // Update the velocity of the particle from the acceleration
    particle->velocity += acceleration.getScaled(deltaTime);

    // Update the orbit angle and radius
    particle->orbitAngle += (particle->orbitVelocity * deltaTime);
    particle->orbitRadius += (particle->orbitRadiusDelta * deltaTime);

    // Cap the angle to the [0.0, 360] range
    while (particle->orbitAngle < 0.0)
        particle->orbitAngle += 360.0;
    while (particle->orbitAngle > 360.0)
        particle->orbitAngle -= 360.0;

    // Update the particle rotation
    particle->rotation += (particle->rotationSpeed * deltaTime);

    // * Update the VESS portions of the particle: the positioning matrix,
    // the rotation & scaling matrix, and the geometry color

    // Calculate the actual position of the particle; this involves combining
    // the particle's stored position with its orbit data. (The particle
    // orbits around the location stored in its 'position' data field.)
    position.set(cos(VS_DEG2RAD(particle->orbitAngle)),
                 sin(VS_DEG2RAD(particle->orbitAngle)),
                 0.0);
    position.scale(particle->orbitRadius);
    position += particle->position;

    // Transform the particle's position from local to global coordinates
    position = particle->emitterMatrix.getPointXform(position);

    // Calculate the particle's current size
    currentSize = particle->initialSize +
        ((particle->finalSize - particle->initialSize) * lifeRatio);

    // See if we're using hardware or software rendering
    if (hardwareShading)
    {
        // Compute the particle's color and place that into the geometry object
        color = (particle->initialColor.getScaled(1.0 - lifeRatio) +
                 particle->finalColor.getScaled(lifeRatio));
        sharedGeom->setData(VS_GEOMETRY_COLORS, 
            particle->geomIndex + 0, color);
        sharedGeom->setData(VS_GEOMETRY_COLORS,
            particle->geomIndex + 1, color);
        sharedGeom->setData(VS_GEOMETRY_COLORS,
            particle->geomIndex + 2, color);
        sharedGeom->setData(VS_GEOMETRY_COLORS,
            particle->geomIndex + 3, color);

        // Texture coordinate 1 holds the particle's X and Y position
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS, 
            particle->geomIndex + 0, vsVector(position[VS_X], position[VS_Y]));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            particle->geomIndex + 1, vsVector(position[VS_X], position[VS_Y]));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            particle->geomIndex + 2, vsVector(position[VS_X], position[VS_Y]));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            particle->geomIndex + 3, vsVector(position[VS_X], position[VS_Y]));

        // Texture coordinate 2 holds the particle's Z position and rotation
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS, 
            particle->geomIndex + 0, vsVector(position[VS_Z], 
            particle->rotation));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            particle->geomIndex + 1, vsVector(position[VS_Z], 
            particle->rotation));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            particle->geomIndex + 2, vsVector(position[VS_Z], 
            particle->rotation));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            particle->geomIndex + 3, vsVector(position[VS_Z], 
            particle->rotation));

        // Texture coordinate 3 holds the particle's size
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS, 
            particle->geomIndex + 0, vsVector(currentSize, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            particle->geomIndex + 1, vsVector(currentSize, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            particle->geomIndex + 2, vsVector(currentSize, 0.0));
        sharedGeom->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            particle->geomIndex + 3, vsVector(currentSize, 0.0));
    }
    else
    {
        // Store the particle's world position in the position transform 
        // attribute
        posMat.setTranslation(position[0], position[1], position[2]);
        particle->positionAttr->setDynamicTransform(posMat);

        // Compute the rotation and scale matrices, and place the combination 
        // of the two into the rotation & scaling transform attribute
        rotMat.setEulerRotation(VS_EULER_ANGLES_ZXY_R,
            particle->rotation, 0.0, 0.0);
        scaleMat.setScale(currentSize, currentSize, currentSize);
        particle->rotScaleAttr->setDynamicTransform(scaleMat * rotMat);

        // Compute the particle's color and place that into the geometry object
        color = (particle->initialColor.getScaled(1.0 - lifeRatio) +
                 particle->finalColor.getScaled(lifeRatio));
        particle->quadGeometry->setData(VS_GEOMETRY_COLORS, 0, color);
    }
}

// ------------------------------------------------------------------------
// Private function
// Computes a random floating-point number in the range [0.0, 1.0]
// ------------------------------------------------------------------------
double vsParticleSystem::getRandom()
{
    // Get a random 4-byte integer and scale it to the [0.0, 1.0] range
    return ((double)(rand()) / (double)(RAND_MAX));
}

// ------------------------------------------------------------------------
// Private function
// Computes a random floating-point number in the range [-1.0, 1.0]
// ------------------------------------------------------------------------
double vsParticleSystem::getRandomVariance()
{
    // Scale the result of getRandom to the desired range
    return ((getRandom() * 2.0) - 1.0);
}

// ------------------------------------------------------------------------
// Private function
// Advances the 'next inactive particle' index if the current one was just
// made active
// ------------------------------------------------------------------------
void vsParticleSystem::findNextInactive()
{
    // If there aren't any inactive particles left, then do nothing
    if (activeParticleCount == particleListSize)
        return;

    // Keep looping through the list of particles until we find an inactive
    // one
    while (((vsParticle *)(particleList[nextInactiveParticleIdx]))->isActive)
    {
        // Move to the next particle
        nextInactiveParticleIdx =
            ((nextInactiveParticleIdx + 1) % particleListSize);
    }
}
