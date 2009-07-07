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

#include "vsParticleSettings.h++"

// ------------------------------------------------------------------------
// Set up the default settings
// ------------------------------------------------------------------------
vsParticleSettings::vsParticleSettings()
{
    // Initial lifetime is 1 second, with no variance
    lifetime = 1.0;
    lifetimeVariance = 0.0;

    // Initial velocity is stationary, with no variance
    initialVelocity.set(0.0, 0.0, 0.0);
    velocityMinAngleVariance = 0.0;
    velocityMaxAngleVariance = 0.0;
    velocitySpeedVariance = 0.0;

    // Initial orbit speed is stationary, with no variance
    orbitSpeed = 0.0;
    orbitSpeedVariance = 0.0;
    orbitRadiusDelta = 0.0;
    orbitRadiusDeltaVariance = 0.0;

    // Initial and final sizes are 1 meter, with no variance
    initialSize = 1.0;
    initialSizeVariance = 0.0;
    finalSize = 1.0;
    finalSizeVariance = 0.0;
    lockSizeVariance = false;

    // Initial rotation and speed are zero, with no variance
    rotation = 0.0;
    rotationVariance = 0.0;
    rotationSpeed = 0.0;
    rotationSpeedVariance = 0.0;

    // Initial and final colors are white and opaque, no variance
    initialColor.set(1.0, 1.0, 1.0, 1.0);
    initialColorVariance.set(0.0, 0.0, 0.0, 0.0);
    finalColor.set(1.0, 1.0, 1.0, 1.0);
    finalColorVariance.set(0.0, 0.0, 0.0, 0.0);
    lockIntraColorVariance = false;
    lockInterColorVariance = false;

    // Acceleration is zero
    acceleration.set(0.0, 0.0, 0.0);

    // Max speed should be infinite (-1.0 acts as a sentinel value)
    maxSpeed = -1.0;

    // Initial render bin is bin zero (the default bin)
    renderBin = 0;
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsParticleSettings::getClassName()
{
    return "vsParticleSettings";
}

// ------------------------------------------------------------------------
// Set the particles' lifetime and potential variance (in seconds)
// ------------------------------------------------------------------------
void vsParticleSettings::setLifetime(double seconds, double var)
{
    // Copy and store the new lifetime values
    lifetime = seconds;
    lifetimeVariance = var;
}

// ------------------------------------------------------------------------
// Get the particles' lifetime and potential variance (in seconds).  NULL
// may be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getLifetime(double *seconds, double *var)
{
    // Copy the lifetime value for each non-NULL parameter given
    if (seconds != NULL)
        *seconds = lifetime;
    if (var != NULL)
        *var = lifetimeVariance;
}

// ------------------------------------------------------------------------
// Set the particles' velocity parameters, including initial velocity,
// minimum and maximum angle variance (applied to the initial velocity's
// direction), and speed variance (applied to the initial velocity's
// magnitude)
// ------------------------------------------------------------------------
void vsParticleSettings::setVelocity(atVector initial, double minAngleVar,
                                     double maxAngleVar, double speedVar)
{
    // Copy and store the new velocity values
    initialVelocity = initial;
    velocityMinAngleVariance = minAngleVar;
    velocityMaxAngleVariance = maxAngleVar;
    velocitySpeedVariance = speedVar;
}

// ------------------------------------------------------------------------
// Get the particles' velocity parameters, as described above.  NULL may
// be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getVelocity(atVector *initial, double *minAngleVar,
                                     double *maxAngleVar, double *speedVar)
{
    // Copy the velocity value for each non-NULL parameter given
    if (initial != NULL)
        *initial = initialVelocity;
    if (minAngleVar != NULL)
        *minAngleVar = velocityMinAngleVariance;
    if (maxAngleVar != NULL)
        *maxAngleVar = velocityMaxAngleVariance;
    if (speedVar != NULL)
        *speedVar = velocitySpeedVariance;
}

// ------------------------------------------------------------------------
// Set the particles' acceleration value
// ------------------------------------------------------------------------
void vsParticleSettings::setAcceleration(atVector accel)
{
    // Copy and store the acceleration
    acceleration = accel;
}

// ------------------------------------------------------------------------
// Get the particles' acceleration value
// ------------------------------------------------------------------------
atVector vsParticleSettings::getAcceleration()
{
    // Return the current acceleration
    return acceleration;
}

// ------------------------------------------------------------------------
// Set the particles' maximum desired speed
// ------------------------------------------------------------------------
void vsParticleSettings::setMaxSpeed(double speed)
{
    // Copy and store the speed
    maxSpeed = speed;
}

// ------------------------------------------------------------------------
// Get the particles' maximum desired speed
// ------------------------------------------------------------------------
double vsParticleSettings::getMaxSpeed()
{
    // Return the current maximum speed
    return maxSpeed;
}

// ------------------------------------------------------------------------
// Set the particles' orbit parameters, including orbit speed and the
// change in radius over the particle's lifetime.  Variance can be set for
// both of these parameters as well
// ------------------------------------------------------------------------
void vsParticleSettings::setOrbit(double speed, double speedVar,
                                  double deltaRadius, double deltaRadiusVar)
{
    // Copy and store the new orbit values
    orbitSpeed = speed;
    orbitSpeedVariance = speedVar;
    orbitRadiusDelta = deltaRadius;
    orbitRadiusDeltaVariance = deltaRadiusVar;
}

// ------------------------------------------------------------------------
// Set the particles' orbit speed and variance, leaving the other orbit
// settings alone
// ------------------------------------------------------------------------
void vsParticleSettings::setOrbitSpeed(double speed, double variance)
{
    // Copy and store the new orbit speed values
    orbitSpeed = speed;
    orbitSpeedVariance = variance;
}

// ------------------------------------------------------------------------
// Set the particles' change in orbit radius and variance, leaving the
// other orbit settings alone
// ------------------------------------------------------------------------
void vsParticleSettings::setOrbitRadiusDelta(double delta, double variance)
{
    // Copy and store the new orbit delta radius values
    orbitRadiusDelta = delta;
    orbitRadiusDeltaVariance = variance;
}

// ------------------------------------------------------------------------
// Get the particles' velocity parameters, as described above.  NULL may
// be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getOrbit(double *speed, double *speedVar,
                                  double *deltaRadius, double *deltaRadiusVar)
{
    // Copy the orbit value for each non-NULL parameter given
    if (speed != NULL)
        *speed = orbitSpeed;
    if (speedVar != NULL)
        *speedVar = orbitSpeedVariance;
    if (deltaRadius != NULL)
        *deltaRadius = orbitRadiusDelta;
    if (deltaRadiusVar != NULL)
        *deltaRadiusVar = orbitRadiusDeltaVariance;
}

// ------------------------------------------------------------------------
// Gets the particles' orbit speed and variance.  NULL values may be passed
// for unneeded parameters
// ------------------------------------------------------------------------
void vsParticleSettings::getOrbitSpeed(double *speed, double *variance)
{
    // Copy the orbit speed value for each non-NULL parameter given
    if (speed)
        *speed = orbitSpeed;
    if (variance)
        *variance = orbitSpeedVariance;
}

// ------------------------------------------------------------------------
// Gets the particles' change in orbit radius and variance.  NULL values
// may be passed for unneeded parameters
// ------------------------------------------------------------------------
void vsParticleSettings::getOrbitRadiusDelta(double *delta, double *variance)
{
    // Copy the orbit delta radius value for each non-NULL parameter given
    if (delta)
        *delta = orbitRadiusDelta;
    if (variance)
        *variance = orbitRadiusDeltaVariance;
}

// ------------------------------------------------------------------------
// Set the particles' size parameters, including the initial and final
// sizes and their variances, and whether or not to use the same random
// value to vary both initial and final sizes
// ------------------------------------------------------------------------
void vsParticleSettings::setSize(double initial, double initialVar,
                                 double final, double finalVar,
                                 bool varLock)
{
    // Copy and store the new size values
    initialSize = initial;
    initialSizeVariance = initialVar;
    finalSize = final;
    finalSizeVariance = finalVar;
    lockSizeVariance = varLock;
}

// ------------------------------------------------------------------------
// Get the particles' size parameters, as described above.  NULL may
// be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getSize(double *initial, double *initialVar,
                                 double *final, double *finalVar,
                                 bool *varLock)
{
    // Copy the size value for each non-NULL parameter given
    if (initial != NULL)
        *initial = initialSize;
    if (initialVar != NULL)
        *initialVar = initialSizeVariance;
    if (final != NULL)
        *final = finalSize;
    if (finalVar != NULL)
        *finalVar = finalSizeVariance;
    if (varLock != NULL)
        *varLock = lockSizeVariance;
}

// ------------------------------------------------------------------------
// Set the particles' rotation parameters (this refers to rotation of the
// particle around the viewing axis).  These include the initial rotation
// angle, rotation speed, and the variances for these two values
// ------------------------------------------------------------------------
void vsParticleSettings::setRotation(double initialAngle, double angleVar,
                                     double speed, double speedVar)
{
    // Copy and store the new rotation values
    rotation = initialAngle;
    rotationVariance = angleVar;
    rotationSpeed = speed;
    rotationSpeedVariance = speedVar;
}

// ------------------------------------------------------------------------
// Set the particles' rotation angle and variance, leaving the other
// rotation settings alone
// ------------------------------------------------------------------------
void vsParticleSettings::setRotationAngle(double angle, double variance)
{
    // Copy and store the new rotation angle values
    rotation = angle;
    rotationVariance = variance;
}

// ------------------------------------------------------------------------
// Set the particles' rotation speed and variance, leaving the other
// rotation settings alone
// ------------------------------------------------------------------------
void vsParticleSettings::setRotationSpeed(double speed, double variance)
{
    // Copy and store the new rotation speed values
    rotationSpeed = speed;
    rotationSpeedVariance = variance;
}

// ------------------------------------------------------------------------
// Get the particles' rotation parameters, as described above.  NULL may
// be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getRotation(double *initialAngle, double *angleVar,
                                     double *speed, double *speedVar)
{
    // Copy the rotation value for each non-NULL parameter given
    if (initialAngle != NULL)
        *initialAngle = rotation;
    if (angleVar != NULL)
        *angleVar = rotationVariance;
    if (speed != NULL)
        *speed = rotationSpeed;
    if (speedVar != NULL)
        *speedVar = rotationSpeedVariance;
}

// ------------------------------------------------------------------------
// Get the particles' rotation angle parameters, as described above.  NULL
// may be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getRotationAngle(double *angle, double *variance)
{
    // Copy the rotation angle value for each non-NULL parameter given
    if (angle != NULL)
        *angle = rotation;
    if (variance != NULL)
        *variance = rotationVariance;
}

// ------------------------------------------------------------------------
// Get the particles' rotation speed parameters, as described above.  NULL
// may be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getRotationSpeed(double *speed, double *variance)
{
    // Copy the rotation speed value for each non-NULL parameter given
    if (speed != NULL)
        *speed = rotationSpeed;
    if (variance != NULL)
        *variance = rotationSpeedVariance;
}

// ------------------------------------------------------------------------
// Set the particles' color parameters, including the initial and final
// colors, the variances for the two colors, and whether or not the same
// random value is used to vary all color elements (intra-color variance
// lock) and/or to vary both initial and final colors (inter-color variance
// lock)
// ------------------------------------------------------------------------
void vsParticleSettings::setColor(atVector initial, atVector initialVar,
                                  atVector final, atVector finalVar,
                                  bool intraLock, bool interLock)
{
    // Copy and store the new color values
    initialColor = initial;
    initialColorVariance = initialVar;
    finalColor = final;
    finalColorVariance = finalVar;
    lockIntraColorVariance = intraLock;
    lockInterColorVariance = interLock;
}

// ------------------------------------------------------------------------
// Get the particles' color parameters, as described above.  NULL may
// be passed for unneeded values
// ------------------------------------------------------------------------
void vsParticleSettings::getColor(atVector *initial, atVector *initialVar,
                                  atVector *final, atVector *finalVar,
                                  bool *intraLock, bool *interLock)
{
    // Copy the color value for each non-NULL parameter given
    if (initial != NULL)
        *initial = initialColor;
    if (initialVar != NULL)
        *initialVar = initialColorVariance;
    if (final != NULL)
        *final = finalColor;
    if (finalVar != NULL)
        *finalVar = finalColorVariance;
    if (intraLock != NULL)
        *intraLock = lockIntraColorVariance;
    if (interLock != NULL)
        *interLock = lockInterColorVariance;
}

// ------------------------------------------------------------------------
// Set the render bin used for drawing particles
// ------------------------------------------------------------------------
void vsParticleSettings::setRenderBin(int newBin)
{
    // Copy and store the new render bin number
    renderBin = newBin;
}

// ------------------------------------------------------------------------
// Get the render bin used for drawing particles
// ------------------------------------------------------------------------
int vsParticleSettings::getRenderBin()
{
    // Return the current render bin number
    return renderBin;
}
