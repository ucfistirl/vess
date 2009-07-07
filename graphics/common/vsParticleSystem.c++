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

    // Create the initial list of vsParticle structures
    particleListSize = 0;
    activeParticleCount = 0;
    primInUse = NULL;
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
    primInUse = NULL;
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
    primInUse = NULL;
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
    primInUse = NULL;
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
    primInUse = NULL;
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
    while (particleList.getNumEntries() > 0)
    {
        // "Destroy" the particle and associated data
        destroyParticle((vsParticle *)(particleList.getEntry(0)));

        // Remove the particle from the list (this will also delete it)
        particleList.removeEntryAtIndex(0);
    }

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

    // If we have a list of primitives in use, delete that now
    if (primInUse != NULL)
    {
        delete [] primInUse;
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
    atMatrix followNodeMatrix;
    double emitInterval;
    double nextEmitTime;
    vsParticle *particle;
    int loop;
    atQuat rotQuat;

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
        particle = (vsParticle *)(particleList.getEntry(loop));
        if (particle->isActive())
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
            particle = (vsParticle *)
                (particleList.getEntry(nextInactiveParticleIdx));
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
    settings.setRenderBin(newBin);
    
    // Set the render bin on the shared geometry node.
    if (sharedGeom != NULL)
        sharedGeom->setRenderBin(settings.getRenderBin());

    // Set the render bin on all existing particles
    for (loop = 0; loop < particleListSize; loop++)
    {
        particle = (vsParticle *)(particleList.getEntry(loop));
        particle->setRenderBin(settings.getRenderBin());
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
        particle = (vsParticle *)(particleList.getEntry(loop));
        if (particle->isActive())
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
void vsParticleSystem::setEmitterPosition(atVector position)
{
    emitterPosition.clearCopy(position);
}

// ------------------------------------------------------------------------
// Gets the location of the center point of the particle emitter
// ------------------------------------------------------------------------
atVector vsParticleSystem::getEmitterPosition()
{
    return emitterPosition;
}

// ------------------------------------------------------------------------
// Sets the velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterVelocity(atVector velocity)
{
    emitterVelocity.clearCopy(velocity);
}

// ------------------------------------------------------------------------
// Gets the velocity of the particle emitter
// ------------------------------------------------------------------------
atVector vsParticleSystem::getEmitterVelocity()
{
    return emitterVelocity;
}

// ------------------------------------------------------------------------
// Sets the orientation of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterOrientation(atQuat orientation)
{
    emitterOrientation = orientation;
}

// ------------------------------------------------------------------------
// Gets the orientation of the particle emitter
// ------------------------------------------------------------------------
atQuat vsParticleSystem::getEmitterOrientation()
{
    return emitterOrientation;
}

// ------------------------------------------------------------------------
// Sets the angular velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setEmitterAngularVelocity(atVector rotationAxis,
    double degreesPerSecond)
{
    emitterAngularVelocityAxis.clearCopy(rotationAxis);
    emitterAngularVelocitySpeed = degreesPerSecond;
}

// ------------------------------------------------------------------------
// Gets the angular velocity of the particle emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getEmitterAngularVelocity(atVector *rotationAxis,
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
    bool *oldPrimInUse;
    vsParticle *particle;

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
        // Set the new list size, but remember the old size for a bit longer
        oldSize = particleListSize;
        particleListSize = maxParticles;

        // See if we're using hardware shading
        if (hardwareShading)
        {
            // Set up a new geometry state, and resize all data lists
            // on the shared geometry object to the appropriate size
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

            // Resize the primitives in use array, and copy the existing
            // data (if any)
            oldPrimInUse = primInUse;
            primInUse = new bool[maxParticles];
            memset(primInUse, 0, sizeof(bool) * maxParticles);
            if (oldPrimInUse != NULL)
            {
                // Copy the old data
                memcpy(primInUse, oldPrimInUse, sizeof(bool) * oldSize);

                // Delete the old array
                delete [] oldPrimInUse;
            }
        }

        // The list is growing; create new particle structures for the new
        // list entries
        for (loop = oldSize; loop < particleListSize; loop++)
            particleList.setEntry(loop, createParticle());
            
        if (hardwareShading)
            sharedGeom->finishNewState();
    }
    else if (maxParticles < particleListSize)
    {
        // Begin a new geometry state on the shared geometry node
        if (hardwareShading)
            sharedGeom->beginNewState();
            
        // The list is shrinking; destroy the particle structures for the
        // disappearing list entries
        while (maxParticles < particleList.getNumEntries())
        {
            // "Destroy" the particle and associated data structures
            particle = (vsParticle *)(particleList.getEntry(maxParticles));
            destroyParticle(particle);

            // Remove the particle from the list (this will also delete it)
            particleList.removeEntryAtIndex(maxParticles);
        }

        // Set the new list size
        particleListSize = maxParticles;

        if (hardwareShading)
        {
            // Resize all data lists on the shared geometry to the appropriate
            // size
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

            // Resize the list of primitives in use
            oldPrimInUse = primInUse;
            primInUse = new bool[maxParticles];
            memset(primInUse, 0, sizeof(bool) * maxParticles);
            if (oldPrimInUse != NULL)
            {
                // Copy the old data
                memcpy(primInUse, oldPrimInUse, sizeof(bool) * maxParticles);

                // Delete the old array
                delete [] oldPrimInUse;
            }
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
void vsParticleSystem::setParticleAcceleration(atVector acceleration)
{
    settings.setAcceleration(acceleration);
}

// ------------------------------------------------------------------------
// Gets the constant acceleration applied to all particles
// ------------------------------------------------------------------------
atVector vsParticleSystem::getParticleAcceleration()
{
    return settings.getAcceleration();
}

// ------------------------------------------------------------------------
// Sets the maximum speed any particle should be allowed to travel. If no
// maximum speed should be enforced, a negative value may be provided
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleMaxSpeed(double speed)
{
    settings.setMaxSpeed(speed);
}

// ------------------------------------------------------------------------
// Gets the maximum speed any particle should be allowed to travel. If no
// maximum speed is to be enforced, a negative value will be returned
// ------------------------------------------------------------------------
double vsParticleSystem::getParticleMaxSpeed()
{
    return settings.getMaxSpeed();
}

// ------------------------------------------------------------------------
// Sets the time (and variance) in seconds for which each particle is
// active
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleLifetime(double seconds, double variance)
{
    // Pass the new lifetime parameters on to the settings object
    settings.setLifetime(seconds, variance);
}

// ------------------------------------------------------------------------
// Gets the time (and variance) in seconds for which each particle is
// active
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleLifetime(double *seconds, 
                                           double *variance)
{
    // Get the lifetime settings from the particle settings object
    settings.getLifetime(seconds, variance);
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
void vsParticleSystem::setParticleVelocity(atVector velocity,
    double minAngleVariance, double maxAngleVariance, double speedVariance)
{
    // Sanity check; maximum angle must be at least as large as minimum
    if (minAngleVariance > maxAngleVariance)
    {
        printf("vsParticleSystem::setParticleVelocity: Maximum angle "
            "variance must be larger than minimum angle variance\n");
        return;
    }

    // Pass the parameters on to the settings object
    settings.setVelocity(velocity, minAngleVariance, maxAngleVariance,
        speedVariance);
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
void vsParticleSystem::getParticleVelocity(atVector *velocity,
    double *minAngleVariance, double *maxAngleVariance, double *speedVariance)
{
    // Get the velocity settings from the particle settings object
    settings.getVelocity(velocity, minAngleVariance, maxAngleVariance,
        speedVariance);
}

// ------------------------------------------------------------------------
// Sets the speed (and variance), in degrees per second, at which the
// particle revolves around the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleOrbitSpeed(double speed, double variance)
{
    // Pass the new orbit speed parameters on to the settings object
    settings.setOrbitSpeed(speed, variance);
}

// ------------------------------------------------------------------------
// Gets the speed (and variance), in degrees per second, at which the
// particle revolves around the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleOrbitSpeed(double *speed, double *variance)
{
    // Get the orbit speed values from the particle settings object
    settings.getOrbitSpeed(speed, variance);
}

// ------------------------------------------------------------------------
// Sets the speed (and variance) at which the particle moves away from (or
// towards, for negative values) the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleOrbitRadiusDelta(double delta,
                                                   double variance)
{
    // Pass the new orbit radius delta parameters on to the settings object
    settings.setOrbitRadiusDelta(delta, variance);
}

// ------------------------------------------------------------------------
// Gets the speed (and variance) at which the particle moves away from (or
// towards, for negative values) the axis of the emitter
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleOrbitRadiusDelta(double *delta,
                                                   double *variance)
{
    // Get the orbit radius values from the particle settings object
    settings.getOrbitRadiusDelta(delta, variance);
}

// ------------------------------------------------------------------------
// Sets the initial and final sizes (and variances) of the particles over
// their lifetimes. The uniform flag specifies if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances.
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleSize(double initial, double initialVariance,
                                       double final, double finalVariance,
                                       bool uniform)
{
    // Pass the new particle size parameters on to the settings object
    settings.setSize(initial, initialVariance, final, finalVariance, uniform);
}

// ------------------------------------------------------------------------
// Gets the initial and final sizes (and variances) of the particles over
// their lifetimes. The uniform flag specifies if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances.
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleSize(double *initial, double *initialVariance,
                                       double *final, double *finalVariance,
                                       bool *uniform)
{
    // Get the size values from the particle settings object
    settings.getSize(initial, initialVariance, final, finalVariance, uniform);
}

// ------------------------------------------------------------------------
// Sets the rotation, in degrees, of each particle around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleRotation(double rotationDegrees,
                                           double variance)
{
    // Pass the new rotation angle parameters on to the settings object
    settings.setRotationAngle(rotationDegrees, variance);
}

// ------------------------------------------------------------------------
// Gets the rotation, in degrees, of each particle around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleRotation(double *rotationDegrees,
                                           double *variance)
{
    // Get the rotation angle values from the particle settings object
    settings.getRotationAngle(rotationDegrees, variance);
}

// ------------------------------------------------------------------------
// Sets the speed, in degrees per second, of the rotation of each particle
// around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleRotationSpeed(double degreesPerSecond,
                                                double variance)
{
    // Pass the new rotation speed parameters on to the settings object
    settings.setRotationSpeed(degreesPerSecond, variance);
}

// ------------------------------------------------------------------------
// Gets the speed, in degrees per second, of the rotation of each particle
// around its X-axis
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleRotationSpeed(double *degreesPerSecond,
                                                double *variance)
{
    // Get the rotation speed values from the particle settings object
    settings.getRotationSpeed(degreesPerSecond, variance);
}

// ------------------------------------------------------------------------
// Sets the initial and final colors (and variances) of the particles over
// their lifetimes. The uniform flags specify if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances. The uniformIntra flag is for forcing variance
// fractions within the color to be the same, while the uniformInter flag
// does the same for variance fractions between the two colors.
// ------------------------------------------------------------------------
void vsParticleSystem::setParticleColor(atVector initial,
                                        atVector initialVariance,
                                        atVector final, atVector finalVariance,
                                        bool uniformIntra, bool uniformInter)
{
    atVector initialColor, finalColor;

    // If no initial alpha specified, force it to be 1
    initialColor.clearCopy(initial);
    initialColor.setSize(4);
    if (initial.getSize() < 4)
        initialColor[3] = 1.0;
    
    // If no final alpha specified, force it to be 1
    finalColor.clearCopy(final);
    finalColor.setSize(4);
    if (final.getSize() < 4)
        finalColor[3] = 1.0;

    // Pass the parameters on to the settings object
    settings.setColor(initialColor, initialVariance,
        finalColor, finalVariance, uniformIntra, uniformInter);
}

// ------------------------------------------------------------------------
// Gets the initial and final colors (and variances) of the particles over
// their lifetimes. The uniform flags specify if the two variance values
// are linked; if true, the same fraction of each variance is used when
// computing variances. The uniformIntra flag is for forcing variance
// fractions within the color to be the same, while the uniformInter flag
// does the same for variance fractions between the two colors.
// ------------------------------------------------------------------------
void vsParticleSystem::getParticleColor(atVector *initial,
                                        atVector *initialVariance,
                                        atVector *final,
                                        atVector *finalVariance,
                                        bool *uniformIntra, bool *uniformInter)
{
    // Get the color values from the particle settings object
    settings.getColor(initial, initialVariance, final, finalVariance,
        uniformIntra, uniformInter);
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

    // Create the particle
    result = new vsParticle();

    // See if we're going to be drawing the particle with the CPU (software)
    // or GPU (hardware)
    if (!hardwareShading)
    {
        // Initialize the particle for software rendering
        result->initSoftware();
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
        primInUse[primIndex] = true;

        // Initialize the particle for hardware rendering
        result->initHardware(sharedGeom, primIndex);
    }

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
    if (particle->isActive())
        deactivateParticle(particle);

    // Dispose of the nodes and attributes
    if (hardwareShading)
    {
        primIndex = particle->getPrimitiveIndex();
        primInUse[primIndex] = false;
    }
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

    atMatrix emitterMat, objectMat, totalMat;
    atMatrix posMat, oriMat;
    atVector pos, initialPos;
    atQuat ori, ori2;
    double frameRatio;
    double distance;
    double headingDegs, pitchDegs;
    double min, max;

    // Don't activate an active particle
    if (particle->isActive())
        return;

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

    // * Randomly compute the particle's initial position based on the shape,
    // orientation, and radii of the emitter

    // Then account for the emitter's shape
    switch (emitterShape)
    {
        case VS_PARTICLESYS_EMITTER_POINT:
            // All particles come from the emitter center point
            initialPos.set(0.0, 0.0, 0.0);
            break;

        case VS_PARTICLESYS_EMITTER_LINE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Randomly determine 'forward' or 'back'
            if (getRandomVariance() > 0.0)
            {
                // forward
                initialPos.set(0.0, distance, 0.0);
            }
            else
            {
                // back
                initialPos.set(0.0, -distance, 0.0);
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
                min = AT_SQR(emitterMinRadius / emitterMaxRadius);
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
            initialPos.set(
                distance * cos(AT_DEG2RAD(headingDegs)),
                distance * sin(AT_DEG2RAD(headingDegs)),
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
                min = AT_SQR(emitterMinRadius / emitterMaxRadius);
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
            pitchDegs = AT_RAD2DEG(asin(getRandomVariance()));

            // Compute the position from the heading and pitch
            initialPos.set(
                cos(AT_DEG2RAD(headingDegs)) * cos(AT_DEG2RAD(pitchDegs)),
                sin(AT_DEG2RAD(headingDegs)) * cos(AT_DEG2RAD(pitchDegs)),
                sin(AT_DEG2RAD(pitchDegs)));
            initialPos.scale(distance);

            break;

        case VS_PARTICLESYS_EMITTER_SQUARE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Compute random values for the two variable coordinates
            initialPos.set(
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance(),
                0.0);

            // Force one (random) coordinate into the min/max radius
            // constraints
            switch ((int)(floor(getRandom() * 4.0)))
            {
                case 0:
                    // Positive X
                    initialPos[0] = distance;
                    break;

                case 1:
                    // Negative X
                    initialPos[0] = -distance;
                    break;

                case 2:
                    // Positive Y
                    initialPos[1] = distance;
                    break;

                default:
                    // Negative Y
                    initialPos[1] = -distance;
                    break;
            }

            break;

        case VS_PARTICLESYS_EMITTER_CUBE:
            // Randomly determine the distance from the emitter center
            distance = emitterMinRadius +
                ((emitterMaxRadius - emitterMinRadius) * getRandom());

            // Compute random values for the three variable coordinates
            initialPos.set(
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance(),
                emitterMaxRadius * getRandomVariance());

            // Force one (random) coordinate into the min/max radius
            // constraints
            switch ((int)(floor(getRandom() * 6.0)))
            {
                case 0:
                    // Positive X
                    initialPos[0] = distance;
                    break;

                case 1:
                    // Negative X
                    initialPos[0] = -distance;
                    break;

                case 2:
                    // Positive Y
                    initialPos[1] = distance;
                    break;

                case 3:
                    // Negative Y
                    initialPos[1] = -distance;
                    break;

                case 4:
                    // Positive Z
                    initialPos[2] = distance;
                    break;

                default:
                    // Negative Z
                    initialPos[2] = -distance;
                    break;
            }

            break;
    }

    // Activate the particle, using the global settings, and the emitter
    // matrix and initial position we just computed.  Also, pass the main
    // component of the particle system (so the particle can attach itself),
    // and take the delay and frame time into account
    particle->activate(&settings, totalMat, initialPos, parentComponent,
        creationDelay, frameTime);

    // Increment the active particle count
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
    if (!particle->isActive())
        return;

    // Deactivate the particle
    particle->deactivate(parentComponent);

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
    // Try to update the particle
    if (particle->update(&settings, deltaTime) == false)
    {
        // The particle has expired, so deactivate it
        deactivateParticle(particle);
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
    vsParticle *particle;
    // If there aren't any inactive particles left, then do nothing
    if (activeParticleCount == particleListSize)
        return;

    // Keep looping through the list of particles until we find an inactive
    // one
    particle = (vsParticle *) particleList.getEntry(nextInactiveParticleIdx);
    while (particle->isActive())
    {
        // Move to the next particle
        nextInactiveParticleIdx =
            ((nextInactiveParticleIdx + 1) % particleListSize);
        particle = (vsParticle *) 
            particleList.getEntry(nextInactiveParticleIdx);
    }
}
