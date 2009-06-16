
#include "vsParticle.h++"
#include "vsBillboardAttribute.h++"


// ------------------------------------------------------------------------
// Constructor.  Initialize data members to a known state
// ------------------------------------------------------------------------
vsParticle::vsParticle()
{
    // Initialize members
    active = false;
    
    mainComponent = NULL;
    positionAttr = NULL;
    rotScaleAttr = NULL;
    quadGeometry = NULL;
    geomIndex = 0;
    
    ageSeconds = 0.0;
    lifetimeSeconds = 0.0;
    
    orbitAngle = 0.0;
    orbitVelocity = 0.0;
    orbitRadius = 0.0;
    orbitRadiusDelta = 0.0;
    
    initialSize = 0.0;
    finalSize = 0.0;

    rotation = 0.0;
    rotationSpeed = 0.0;

    sharedGeometry = NULL;
}

// ------------------------------------------------------------------------
// Destructor.  Clean up the objects that we own
// ------------------------------------------------------------------------
vsParticle::~vsParticle()
{
    if (mainComponent != NULL)
        vsObject::unrefDelete(mainComponent);
    if (positionAttr != NULL)
        vsObject::unrefDelete(positionAttr);
    if (rotScaleAttr != NULL)
        vsObject::unrefDelete(rotScaleAttr);
    if (quadGeometry != NULL)
        vsObject::unrefDelete(quadGeometry);
    if (sharedGeometry != NULL)
        vsObject::unrefDelete(sharedGeometry);
}

// ------------------------------------------------------------------------
// Protected function
// Computes a random floating-point number in the range [0.0, 1.0]
// ------------------------------------------------------------------------
double vsParticle::getRandom()
{
    // Get a random 4-byte integer and scale it to the [0.0, 1.0] range
    return ((double)(rand()) / (double)(RAND_MAX));
}

// ------------------------------------------------------------------------
// Protected function
// Computes a random floating-point number in the range [-1.0, 1.0]
// ------------------------------------------------------------------------
double vsParticle::getRandomVariance()
{
    // Scale the result of getRandom to the desired range
    return ((getRandom() * 2.0) - 1.0);
}

// ------------------------------------------------------------------------
// Print the name of this class
// ------------------------------------------------------------------------
const char *vsParticle::getClassName()
{
    return "vsParticle";
}   

// ------------------------------------------------------------------------
// Initialize the particle structure for software rendering
// ------------------------------------------------------------------------
void vsParticle::initSoftware()
{
    vsComponent *translationComponent, *billboardComponent;
    vsComponent *rotScaleComponent;
    vsGeometry *geometry;
    vsTransformAttribute *translateAttr, *rotScaleAttr;
    vsBillboardAttribute *bbAttr;

    // Create the component with the particle position transform
    translationComponent = new vsComponent();
    translateAttr = new vsTransformAttribute();
    translationComponent->addAttribute(translateAttr);

    // Create the component that houses the billboard attribute
    billboardComponent = new vsComponent();
    bbAttr = new vsBillboardAttribute();
    bbAttr->setMode(VS_BILLBOARD_ROT_POINT_EYE);
    bbAttr->setFrontDirection(atVector(0.0, 0.0, 1.0));
    bbAttr->setAxis(atVector(0.0, 1.0, 0.0));
    billboardComponent->addAttribute(bbAttr);

    // Create the component that houses the rotation & scaling matrix
    rotScaleComponent = new vsComponent();
    rotScaleAttr = new vsTransformAttribute();
    rotScaleComponent->addAttribute(rotScaleAttr);

    // Create the particle's geometry
    geometry = new vsGeometry();
    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
    geometry->setPrimitiveCount(1);

    geometry->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 4);
    geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 0, atVector(-0.5, -0.5, 0.0));
    geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 1, atVector( 0.5, -0.5, 0.0));
    geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 2, atVector( 0.5,  0.5, 0.0));
    geometry->setData(VS_GEOMETRY_VERTEX_COORDS, 3, atVector(-0.5,  0.5, 0.0));

    geometry->setBinding(VS_GEOMETRY_NORMALS, VS_GEOMETRY_BIND_OVERALL);
    geometry->setDataListSize(VS_GEOMETRY_NORMALS, 1);
    geometry->setData(VS_GEOMETRY_NORMALS, 0, atVector(0.0, 0.0, 1.0));

    geometry->setBinding(VS_GEOMETRY_COLORS, VS_GEOMETRY_BIND_OVERALL);
    geometry->setDataListSize(VS_GEOMETRY_COLORS, 1);
    geometry->setData(VS_GEOMETRY_COLORS, 0, atVector(1.0, 1.0, 1.0, 1.0));

    geometry->setBinding(VS_GEOMETRY_TEXTURE_COORDS,
        VS_GEOMETRY_BIND_PER_VERTEX);
    geometry->setDataListSize(VS_GEOMETRY_TEXTURE_COORDS, 4);
    geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 0, atVector(0.0, 0.0));
    geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 1, atVector(1.0, 0.0));
    geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 2, atVector(1.0, 1.0));
    geometry->setData(VS_GEOMETRY_TEXTURE_COORDS, 3, atVector(0.0, 1.0));

    geometry->enableLighting();

    geometry->setIntersectValue(0x00000001);

    // Connect the chain of nodes together
    translationComponent->addChild(billboardComponent);
    billboardComponent->addChild(rotScaleComponent);
    rotScaleComponent->addChild(geometry);

    // Keep the objects that we'll need later
    mainComponent = translationComponent;
    mainComponent->ref();
    positionAttr = translateAttr;
    positionAttr->ref();
    rotScaleAttr = rotScaleAttr;
    rotScaleAttr->ref();
    quadGeometry = geometry;
    quadGeometry->ref();

    // Set the particle to be inactive
    active = false;

    // Remember that we're not using hardware rendering
    hardwareShading = false;
}

// ------------------------------------------------------------------------
// Initialize the particle structure for hardware rendering
// ------------------------------------------------------------------------
void vsParticle::initHardware(vsDynamicGeometry *sharedGeom, int primIndex)
{
    // Grab and reference the shared geometry object
    sharedGeometry = sharedGeom;
    sharedGeometry->ref();

    // Set the geometry index and primitive index (this particle's position
    // in the shared geometry)
    primitiveIndex = primIndex;
    geomIndex = primitiveIndex * 4;

    // Set up the particle's position, normal, color and texture coords
    sharedGeometry->setData(VS_GEOMETRY_VERTEX_COORDS, geomIndex + 0,
        atVector(-0.5, -0.5, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_VERTEX_COORDS, geomIndex + 1,
        atVector( 0.5, -0.5, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_VERTEX_COORDS, geomIndex + 2,
        atVector( 0.5,  0.5, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_VERTEX_COORDS, geomIndex + 3,
        atVector(-0.5,  0.5, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_NORMALS, geomIndex + 0,
        atVector(0.0, 0.0, 1.0));
    sharedGeometry->setData(VS_GEOMETRY_NORMALS, geomIndex + 1,
        atVector(0.0, 0.0, 1.0));
    sharedGeometry->setData(VS_GEOMETRY_NORMALS, geomIndex + 2,
        atVector(0.0, 0.0, 1.0));
    sharedGeometry->setData(VS_GEOMETRY_NORMALS, geomIndex + 3,
        atVector(0.0, 0.0, 1.0));
    sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 0,
        atVector(0.0, 0.0, 0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 1,
        atVector(0.0, 0.0, 0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 2,
        atVector(0.0, 0.0, 0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 3,
        atVector(0.0, 0.0, 0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE0_COORDS, geomIndex + 0,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE0_COORDS, geomIndex + 1,
        atVector(1.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE0_COORDS, geomIndex + 2,
        atVector(1.0, 1.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE0_COORDS, geomIndex + 3,
        atVector(0.0, 1.0));

    // Texture coordinate 1 holds the particle's X and Y position
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS, geomIndex + 0,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS, geomIndex + 1,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS, geomIndex + 2,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS, geomIndex + 3,
        atVector(0.0, 0.0));

    // Texture coordinate 2 holds the particle's Z position and rotation
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS, geomIndex + 0,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS, geomIndex + 1,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS, geomIndex + 2,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS, geomIndex + 3,
        atVector(0.0, 0.0));

    // Texture coordinate 3 holds the particle's size
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS, geomIndex + 0,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS, geomIndex + 1,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS, geomIndex + 2,
        atVector(0.0, 0.0));
    sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS, geomIndex + 3,
        atVector(0.0, 0.0));

    // Set the particle to be inactive
    active = false;

    // Remember that we're using hardware rendering
    hardwareShading = true;
}

// ------------------------------------------------------------------------
// Print the name of this class
// ------------------------------------------------------------------------
void vsParticle::setRenderBin(int newBin)
{
    // See if we're using software rendering (hardware rendering uses a
    // shared geometry object that is managed by the particle system itself)
    if (!hardwareShading)
    {
        // Change the render bin on our quad
        quadGeometry->setRenderBin(newBin);
    }
}

// ------------------------------------------------------------------------
// Print the name of this class
// ------------------------------------------------------------------------
void vsParticle::activate(vsParticleSettings *settings, atMatrix emitMatrix,
                          atVector initialPos, vsComponent *parentComponent,
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

    double headingDegs, pitchDegs;
    atVector direction;
    atVector xDir, yDir, zDir;
    double speed;
    double initial;
    double initialVariance;
    double delta;
    double deltaVariance;
    double final;
    double finalVariance;
    double variance;
    atVector initialVec;
    atVector initialVecVar;
    atVector finalVec;
    atVector finalVecVar;
    atVector pos;
    bool interLock;
    bool intraLock;
    int loop;
    double variances[4];
    double min, max;

    // Don't activate an active particle
    if (active)
        return;

    // Attach the particle's geometry to the particle system's master
    // component
    if (!hardwareShading)
        parentComponent->addChild(mainComponent);

    // Determine the particle's lifetime and set it to zero age
    settings->getLifetime(&initial, &initialVariance);
    lifetimeSeconds = initial + (initialVariance * getRandomVariance());
    ageSeconds = 0.0;

    // The lifetime must be a positive number; otherwise, no one will ever
    // see the particle.
    if (lifetimeSeconds < AT_DEFAULT_TOLERANCE)
        lifetimeSeconds = AT_DEFAULT_TOLERANCE;

    // Copy the emitter matrix and initial position (these were computed by
    // the main particle system for us)
    emitterMatrix = emitMatrix;
    position = initialPos;
    
    // * Randomly compute the particle's velocity, based on the given angle
    // and speed variances
    settings->getVelocity(&initialVec, NULL, NULL, NULL);
    direction = initialVec.getNormalized();
    speed = initialVec.getMagnitude();

    // Check for no initial velocity
    if (speed < AT_DEFAULT_TOLERANCE)
        velocity.set(0.0, 0.0, 0.0);
    else
    {
        // * Compute two vectors orthogonal to the original direction vector
        // and to each other

        // Create any unit vector and force it to be orthogonal
        xDir.set(1.0, 0.0, 0.0);
        xDir -= direction.getScaled(direction.getDotProduct(xDir));
        if (xDir.getMagnitude() < AT_DEFAULT_TOLERANCE)
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
        settings->getVelocity(NULL, &min, &max, NULL);
        min = cos(AT_DEG2RAD(min));
        max = cos(AT_DEG2RAD(max));
        pitchDegs = min + ((max - min) * getRandom());
        pitchDegs = AT_RAD2DEG(acos(pitchDegs));

        // Using the original direction as a Z axis, and our two new
        // directions as X and Y axes, construct a new direction using the
        // randomly-computed heading and pitch
        zDir = direction;
        direction =
            xDir.getScaled(cos(AT_DEG2RAD(headingDegs)) *
                sin(AT_DEG2RAD(pitchDegs))) +
            yDir.getScaled(sin(AT_DEG2RAD(headingDegs)) *
                sin(AT_DEG2RAD(pitchDegs))) +
            zDir.getScaled(cos(AT_DEG2RAD(pitchDegs)));

        // Factor the speed back in, modified by the speed variance
        settings->getVelocity(NULL, NULL, NULL, &initialVariance);
        direction.scale(speed + (initialVariance * getRandomVariance()));

        // Set the result as the particle's velocity
        velocity = direction;
    }

    // * Compute the orbit parameters

    // Calculate the initial orbit radius by calculating the distance from
    // the particle and the origin, after the particle is projected onto the
    // XY plane
    pos = position;
    pos[2] = 0.0;
    orbitRadius = pos.getMagnitude();

    // Calculate the initial orbit angle by taking the initial position of
    // the particle and checking its angle with the X axis when projected 
    // into the XY plane.
    // Error checking: If the orbit radius is zero, then the orbit angle
    // calculation probably won't come out right. (atan2 tends to spit out
    // NAN values when given two zeroes.) Set the orbit angle to a default
    // value when this occurs.
    if (orbitRadius < AT_DEFAULT_TOLERANCE)
        orbitAngle = 0.0;
    else
        orbitAngle = AT_RAD2DEG(atan2(pos[1], pos[0]));

    // Randomly compute the orbit velocity and orbit radius delta velocity
    settings->getOrbit(&initial, &initialVariance, &delta, &deltaVariance);
    orbitVelocity = initial + (initialVariance * getRandomVariance());
    orbitRadiusDelta = delta + (deltaVariance * getRandomVariance());

    // Since the particle's x and y-coordinate information is now effectively
    // stored in the orbit data, remove that same information from the
    // 'current position' field, as otherwise when we go to reconstruct the
    // particle position we'll be adding in that data twice.
    position[0] = 0.0;
    position[1] = 0.0;

    // * Randomly compute the particle's sizes over its lifetime
    settings->getSize(&initial, &initialVariance, &final, &finalVariance,
        &interLock);
    variance = getRandomVariance();
    initialSize = initial + (initialVariance * variance);
    if (initialSize < 0.0)
        initialSize = 0.0;

    // Only compute a new variance if the size variance isn't uniform
    if (!interLock)
        variance = getRandomVariance();
    finalSize = final + (finalVariance * variance);
    if (finalSize < 0.0)
        finalSize = 0.0;

    // * Randomly compute the particle's rotation
    settings->getRotation(&initial, &variance, &delta, &deltaVariance);
    rotation = initial + (variance * getRandomVariance());
    rotationSpeed = delta + (deltaVariance * getRandomVariance());

    // * Randomly compute the particle's colors over its lifetime

    // Create an overall variance for the initial color, in case we're using
    // the intra-color lock mode
    settings->getColor(&initialVec, &initialVecVar, &finalVec, &finalVecVar,
        &intraLock, &interLock);
    variance = getRandomVariance();

    // Compute the initial color
    for (loop = 0; loop < 4; loop++)
    {
        if (intraLock)
        {
            // Use the same variance for each component of this color
            initialColor[loop] = initialVec[loop] +
                (initialVecVar[loop] * variance);
        }
        else
        {
            // Use a new variance for each component of this color, but store
            // the variances in case we need them for the final color
            variances[loop] = getRandomVariance();
            initialColor[loop] = initialVec[loop] +
                (initialVecVar[loop] * variances[loop]);
        }
    }
    
    // If there is no lock between the two colors, then get a new overall
    // variance for the final color
    if (!interLock)
        variance = getRandomVariance();

    // Compute the final color
    for (loop = 0; loop < 4; loop++)
    {
        if (intraLock)
        {
            // Use the same variance for each component of this color
            finalColor[loop] = finalVec[loop] +
                (finalVecVar[loop] * variance);
        }
        else
        {
            // Use a separate variance for each component of this color. If
            // we're in inter-color lock mode, use the variances from the
            // initial color; otherwise, make new ones.
            if (interLock)
                finalColor[loop] = finalVec[loop] +
                    (finalVecVar[loop] * variances[loop]);
            else
                finalColor[loop] = finalVec[loop] +
                    (finalVecVar[loop] * getRandomVariance());
        }
    }

    // If we're using software rendering, set the render bin on our quad
    if (!hardwareShading)
        quadGeometry->setRenderBin(settings->getRenderBin());

    // * Use the particle update function to advance the particle in time to
    // be in sync with the rest of the active particles 
    update(settings, frameTime - creationDelay);

    // Mark the particle as activated
    active = true;
}

// ------------------------------------------------------------------------
// Print the name of this class
// ------------------------------------------------------------------------
void vsParticle::deactivate(vsComponent *parentComponent)
{
    // Remove the particle node chain from the system's master component
    if (hardwareShading)
    {
        // Hide the particle in the primitive list, by setting it's alpha
        // value to zero
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 0,
            atVector(0.0, 0.0, 0.0, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 1,
            atVector(0.0, 0.0, 0.0, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 2,
            atVector(0.0, 0.0, 0.0, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 3,
            atVector(0.0, 0.0, 0.0, 0.0));
    }
    else
    {
        // When using software, we attach a chain of three nodes rooted
        // at "mainComponent".  Detach this chain to deactivate the
        // particle.
        parentComponent->removeChild(mainComponent);
    }

    // Mark this particle as inactive
    active = false;
}

// ------------------------------------------------------------------------
// Return whether or not this particle is active
// ------------------------------------------------------------------------
bool vsParticle::isActive()
{
    return active;
}

// ------------------------------------------------------------------------
// Return the index of the primitive within the shared geometry that this
// particle is using (in software rendering mode, we return -1)
// ------------------------------------------------------------------------
int vsParticle::getPrimitiveIndex()
{
    // Inactive particles don't use any primitives
    if (active)
    {
        // If we're not hardware shading, we make our own primitive
        if (hardwareShading)
            return primitiveIndex;
    }

    // We're either inactive, or not using hardware shading, so return -1
    return -1;
}

// ------------------------------------------------------------------------
// Print the name of this class
// ------------------------------------------------------------------------
bool vsParticle::update(vsParticleSettings *settings, double deltaTime)
{
    atVector acceleration;
    atVector orbitPos;
    atMatrix posMat, rotMat, scaleMat;
    double currentSize;
    atVector color;
    double lifeRatio;

    // * Update the various data fields of the particle

    // Increase the age of the particle. If this causes the particle to reach
    // the end of its lifespan, return false to indicate that the particle
    // has expired
    ageSeconds += deltaTime;
    if (ageSeconds > lifetimeSeconds)
        return false;

    // Compute the fraction of its lifetime that the particle has gone
    // through; we'll need this for interpolation later
    lifeRatio = (ageSeconds / lifetimeSeconds);

    // Compute the acceleration vector, taking into account the coordinate
    // system of the particle. (The particle's stored position is always in
    // its own coordinate system, which is the coordinate system of the
    // emitter at the point in time when the particle was created, while the
    // acceleration is in global coordinates.)
    acceleration = emitterMatrix.getInverse().
        getVectorXform(settings->getAcceleration());

    // Compute the new position of the particle, using both the particle's
    // current velocity and the constant acceleration
    position += (velocity.getScaled(deltaTime) +
        acceleration.getScaled(AT_SQR(deltaTime)));

    // Update the velocity of the particle from the acceleration
    velocity += acceleration.getScaled(deltaTime);

    // Update the orbit angle and radius
    orbitAngle += (orbitVelocity * deltaTime);
    orbitRadius += (orbitRadiusDelta * deltaTime);

    // Cap the angle to the [0.0, 360] range
    while (orbitAngle < 0.0)
        orbitAngle += 360.0;
    while (orbitAngle > 360.0)
        orbitAngle -= 360.0;

    // Update the particle rotation
    rotation += (rotationSpeed * deltaTime);

    // * Update the VESS portions of the particle: the positioning matrix,
    // the rotation & scaling matrix, and the geometry color

    // Calculate the actual position of the particle; this involves combining
    // the particle's stored position with its orbit data. (The particle
    // orbits around the location stored in its 'position' data field.)
    orbitPos.set(cos(AT_DEG2RAD(orbitAngle)), sin(AT_DEG2RAD(orbitAngle)),
                 0.0);
    orbitPos.scale(orbitRadius);
    orbitPos += position;

    // Transform the particle's position from local to global coordinates
    orbitPos = emitterMatrix.getPointXform(orbitPos);

    // Calculate the particle's current size
    currentSize = initialSize + ((finalSize - initialSize) * lifeRatio);

    // See if we're using hardware or software rendering
    if (hardwareShading)
    {
        // Compute the particle's color and place that into the geometry object
        color = (initialColor.getScaled(1.0 - lifeRatio) +
                 finalColor.getScaled(lifeRatio));
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 0, color);
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 1, color);
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 2, color);
        sharedGeometry->setData(VS_GEOMETRY_COLORS, geomIndex + 3, color);

        // Texture coordinate 1 holds the particle's X and Y position
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS, 
            geomIndex + 0, atVector(orbitPos[AT_X], orbitPos[AT_Y]));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            geomIndex + 1, atVector(orbitPos[AT_X], orbitPos[AT_Y]));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            geomIndex + 2, atVector(orbitPos[AT_X], orbitPos[AT_Y]));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE1_COORDS,
            geomIndex + 3, atVector(orbitPos[AT_X], orbitPos[AT_Y]));
    
        // Texture coordinate 2 holds the particle's Z position and rotation
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            geomIndex + 0, atVector(orbitPos[AT_Z],
            rotation));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            geomIndex + 1, atVector(orbitPos[AT_Z], 
            rotation));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS,
            geomIndex + 2, atVector(orbitPos[AT_Z],
            rotation));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE2_COORDS, 
            geomIndex + 3, atVector(orbitPos[AT_Z],
            rotation));

        // Texture coordinate 3 holds the particle's size
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            geomIndex + 0, atVector(currentSize, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            geomIndex + 1, atVector(currentSize, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            geomIndex + 2, atVector(currentSize, 0.0));
        sharedGeometry->setData(VS_GEOMETRY_TEXTURE3_COORDS,
            geomIndex + 3, atVector(currentSize, 0.0));
    }
    else
    {
        // Store the particle's world position in the position transform
        // attribute
        posMat.setTranslation(orbitPos[0], orbitPos[1], orbitPos[2]);
        positionAttr->setDynamicTransform(posMat);

        // Compute the rotation and scale matrices, and place the combination
        // of the two into the rotation & scaling transform attribute
        rotMat.setEulerRotation(AT_EULER_ANGLES_ZXY_R,
            rotation, 0.0, 0.0);
        scaleMat.setScale(currentSize, currentSize, currentSize);
        rotScaleAttr->setDynamicTransform(scaleMat * rotMat);

        // Compute the particle's color and place that into the geometry object
        color = (initialColor.getScaled(1.0 - lifeRatio) +
                 finalColor.getScaled(lifeRatio));
        quadGeometry->setData(VS_GEOMETRY_COLORS, 0, color);
    }

    // Return true to indicate a successful update
    return true;
}

