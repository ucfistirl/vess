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
//    VESS Module:  vsArticulationGlove.c++
//
//    Description:  Device to keep track of the state of a VR articulation
//                  glove.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsArticulationGlove.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor.  Initializes all the vsInputAxis and vsInputButton objects.
// The parameter indicates whether or not the distal interphalangial joints
// should be estimated or not.  true means yes, false means no.
// ------------------------------------------------------------------------
vsArticulationGlove::vsArticulationGlove(bool estDistal)
{
    int i;

    // Save the estimateDistal parameter
    estimateDistal = estDistal;

    // Initialize the calibrating flag to false (not calibrating)
    calibrating = false;

    // Construct sensor axes, start with a reasonable range, which will
    // be more closely calibrated later
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
    {
        sensors[i] = new vsInputAxis(1, 255);
        sensors[i]->ref();
    }

    // Construct input buttons
    for (i = 0; i < VS_AG_NUM_BUTTONS; i++)
    {
        buttons[i] = new vsInputButton();
        buttons[i]->ref();
    }
}

// ------------------------------------------------------------------------
// Destructor.
// ------------------------------------------------------------------------
vsArticulationGlove::~vsArticulationGlove()
{
    int i;

    // Destroy sensor axes
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        vsObject::unrefDelete(sensors[i]);

    // Destroy buttons
    for (i = 0; i < VS_AG_NUM_BUTTONS; i++)
        vsObject::unrefDelete(buttons[i]);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsArticulationGlove::getClassName()
{
    return "vsArticulationGlove";
}

// ------------------------------------------------------------------------
// Updates the joint angles from the current sensor values.  Call this 
// method after updating all the sensor values.
// ------------------------------------------------------------------------
void vsArticulationGlove::update()
{
    int    i;
    double deg1, deg2;
    double middleRingAbd;
    atQuat quat1, quat2;

    // A calibrated glove has all axis idle positions at the beginning of
    // the range of motion (joints fully extended).  If we're currently 
    // calibrating, we need to see if we need to update any idle positions
    // to have a smaller axis value.  If we're not calibrating, we need to
    // make sure the axis values don't go beyond the idle position.
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
    {
        // See if the axis value is negative
        if (sensors[i]->getPosition() < 0.0)
        {
            // Adjust the idle position if we're calibrating, or clamp
            // the axis value to zero if not
            if (calibrating)
            {
                // Change the idle position to the current sensor position 
                // if the current position is less than the current idle 
                // position
                sensors[i]->setIdlePosition();
            }
            else
            {
                // The axis is behind the idle position, so clamp it to
                // the idle position
                sensors[i]->setPosition(sensors[i]->getIdlePosition());
            }
        }
    }

    // Read the thumb abduction sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_ABD]->getPosition() *
        VS_AG_THUMB_ABD_LIMIT;
        
    // Read the thumb MJ sensor
    deg2 = sensors[VS_AG_SENSOR_THUMB_MJ]->getPosition() * 
        VS_AG_THUMB_MJ_LIMIT;
        
    // Compute the thumb CMC joint angle
    quat1.setAxisAngleRotation(0, 0, 1, deg1);
    quat2.setAxisAngleRotation(0, 1, 0, -deg2);
    joints[VS_AG_JOINT_THUMB_MJ] = quat2 * quat1;

    // Read the thumb MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_MPJ]->getPosition() * 
        VS_AG_THUMB_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_THUMB_MPJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the thumb IJ sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_IJ]->getPosition() * 
        VS_AG_THUMB_IJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_THUMB_IJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the index MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_INDEX_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;
    
    // Read the index/middle abduction sensor (use this to rotate the index 
    // finger to the left)
    deg2 = sensors[VS_AG_SENSOR_MIDDLE_ABD]->getPosition() * 
        VS_AG_INDEX_MIDDLE_ABD_LIMIT;

    // Compute the joint angle
    quat1.setAxisAngleRotation(1, 0, 0, deg1);
    quat2.setAxisAngleRotation(0, 0, 1, -deg2);
    joints[VS_AG_JOINT_INDEX_MPJ] = quat1 * quat2;
 
    // Read the index PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_INDEX_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_INDEX_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

    // Estimate the distal joint if set to do so
    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_INDEX_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the middle MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_MIDDLE_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_MIDDLE_MPJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the middle PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_MIDDLE_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_MIDDLE_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

    // Estimate the distal joint if set to do so
    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_MIDDLE_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the ring MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_RING_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;
    
    // Read the middle/ring abduction sensor
    deg2 = sensors[VS_AG_SENSOR_RING_ABD]->getPosition() *
        VS_AG_MIDDLE_RING_ABD_LIMIT;
    middleRingAbd = deg2;

    // Compute the joint angle
    quat1.setAxisAngleRotation(1, 0, 0, deg1);
    quat2.setAxisAngleRotation(0, 0, 1, deg2);
    joints[VS_AG_JOINT_RING_MPJ] = quat1 * quat2;

    // Read the ring PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_RING_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_RING_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

    // Estimate the distal joint if set to do so
    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_RING_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_PINKY_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;
    
    // Read the ring/pinky abduction sensor, and add it to the abduction
    // sensor reading of the middle/ring fingers
    deg2 = sensors[VS_AG_SENSOR_PINKY_ABD]->getPosition() *
        VS_AG_RING_PINKY_ABD_LIMIT + middleRingAbd;

    // Compute the joint angle
    quat1.setAxisAngleRotation(1, 0, 0, deg1);
    quat2.setAxisAngleRotation(0, 0, 1, deg2);
    joints[VS_AG_JOINT_PINKY_MPJ] = quat1 * quat2;

    // Read the PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_PINKY_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_PINKY_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

    // Estimate the distal joint if set to do so
    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_PINKY_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Only use the distal sensors if we're not configured to estimate
    // the distal joints
    if (!estimateDistal)
    {
        // Read the index DIJ sensor
        deg1 = sensors[VS_AG_SENSOR_INDEX_DIJ]->getPosition() * 
            VS_AG_DIJ_LIMIT;

        // Compute the joint angle
        joints[VS_AG_JOINT_INDEX_DIJ].setAxisAngleRotation(1, 0, 0, deg1);

        // Read the middle DIJ sensor
        deg1 = sensors[VS_AG_SENSOR_MIDDLE_DIJ]->getPosition() *
            VS_AG_DIJ_LIMIT;

        // Compute the joint angle
        joints[VS_AG_JOINT_MIDDLE_DIJ].setAxisAngleRotation(1, 0, 0, deg1);

        // Read the ring DIJ sensor
        deg1 = sensors[VS_AG_SENSOR_RING_DIJ]->getPosition() *
            VS_AG_DIJ_LIMIT;

        // Compute the joint angle
        joints[VS_AG_JOINT_RING_DIJ].setAxisAngleRotation(1, 0, 0, deg1);

        // Read the pinky DIJ sensor
        deg1 = sensors[VS_AG_SENSOR_PINKY_DIJ]->getPosition() *
            VS_AG_DIJ_LIMIT;

        // Compute the joint angle
        joints[VS_AG_JOINT_PINKY_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the wrist sensors, the scale factors and offsets correspond to
    // the typical human range of motion for wrist flexion/extension and 
    // abduction/adduction
    deg1 = sensors[VS_AG_SENSOR_WRIST_PITCH]->getPosition() * 
        -VS_AG_WRIST_FLEX_SCALE + VS_AG_WRIST_FLEX_OFFSET;
    deg2 = sensors[VS_AG_SENSOR_WRIST_YAW]->getPosition() * 
        -VS_AG_WRIST_ABD_SCALE + VS_AG_WRIST_ABD_OFFSET;

    // Compute the joint angle
    quat1.setAxisAngleRotation(1, 0, 0, deg1);
    quat2.setAxisAngleRotation(0, 0, 1, deg2);
    joints[VS_AG_JOINT_WRIST] = quat2 * quat1;

    // Palm Arch not yet supported
    joints[VS_AG_JOINT_PALM_ARCH].setAxisAngleRotation(0, 0, 0, 1);

    // Update all buttons and axes
    vsIODevice::update();
}

// ------------------------------------------------------------------------
// Returns the number of vsInputAxis objects (sensors)
// ------------------------------------------------------------------------
int vsArticulationGlove::getNumAxes()
{
    return VS_AG_NUM_SENSORS;
}

// ------------------------------------------------------------------------
// Returns the number of vsInputButton objects
// ------------------------------------------------------------------------
int vsArticulationGlove::getNumButtons()
{
    return VS_AG_NUM_BUTTONS;
}

// ------------------------------------------------------------------------
// Returns the vsInputAxis object corresponding to the given index
// ------------------------------------------------------------------------
vsInputAxis *vsArticulationGlove::getAxis(int index)
{
    // Verify the index is valid, return NULL if not
    if ((index >= 0) && (index < VS_AG_NUM_SENSORS))
        return sensors[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns the vsInputButton object corresponding to the given index
// ------------------------------------------------------------------------
vsInputButton *vsArticulationGlove::getButton(int index)
{
    // Verify the index is valid, return NULL if not
    if ((index >= 0) && (index < VS_AG_NUM_BUTTONS))
        return buttons[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns the atQuat corresponding to the given joint index
// ------------------------------------------------------------------------
atQuat vsArticulationGlove::getJoint(int index)
{
    // Verify the index is valid, return NULL if not
    if ((index >= 0) && (index < VS_AG_NUM_JOINTS))
        return joints[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Saves the current calibration information for all axes to a file
// ------------------------------------------------------------------------
void vsArticulationGlove::saveCalibration(char *filename)
{
    FILE *fp;
    int  i;
    double idle, min, max;

    // Open a file with the given name for writing
    fp = fopen(filename, "w");

    // Bail out if the file doesn't open
    if (!fp)
    {
        printf("vsArticulationGlove::saveCalibration:\n");
        printf("    Unable to create file %s\n", filename);
        return;
    }

    // For each sensor axis...
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
    {
        // Get the axis range and write it to the file on its own line
        idle = sensors[i]->getIdlePosition();
        sensors[i]->getRange(&min, &max);
        fprintf(fp, "%0.10lf %0.10lf %0.10lf\n", idle, min, max);
    }

    // Close the file
    fclose(fp);
}

// ------------------------------------------------------------------------
// Retrieves saved calibration information for all axes from a file
// ------------------------------------------------------------------------
void vsArticulationGlove::loadCalibration(char *filename)
{
    FILE *fp;
    int  i;
    double idle, min, max;

    // Open a file with the given name for reading
    fp = fopen(filename, "r");

    // Bail out if the file doesn't open
    if (!fp)
    {
        printf("vsArticulationGlove::loadCalibration:\n");
        printf("    Unable to open file %s\n", filename);
        return;
    }

    // For each sensor axis...
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
    {
        // Get the axis range and write it to the file on its own line
        fscanf(fp, "%lf %lf %lf\n", &idle, &min, &max);
        sensors[i]->setIdlePosition(idle);
        sensors[i]->setRange(min, max);
    }

    // Close the file
    fclose(fp);
}

// ------------------------------------------------------------------------
// Enables/disables passive calibration for all sensors
// ------------------------------------------------------------------------
void vsArticulationGlove::passiveCalibrate(bool enable)
{
    int i;
    double axisMin, axisMax, idlePos;

    // The calibration procedure for the glove is as follows.  Set the
    // idle position of each sensor to maximum (255), and enable passive
    // calibration on the sensor axis.  As each sensor measurement is
    // taken (in update()), if the sensor value is less than the current
    // idle position, the idle position is adjusted to match.  The result
    // is that each sensor of the glove will have an idle position of the 
    // minimum sensor value and an axis maximum of the maximum sensor value 
    // based on the user's range of motion.
    //
    // The proper calibration procedure is to extend all joints of the hand
    // as much as possible, enable calibration, flex all joints as much as
    // possible (make a tight fist with the glove), disable calibration.
    // This gives good results.

    // Check the enable flag to see if we should enable or disable calibration
    if (enable)
    {
        // Enabling calibration.  Set the flag to true.
        calibrating = true;

        // Set all idle positions to the maximum sensor value and enable 
        // calibration on the input axes
        for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        {
            // The idle position will need to be on the opposite end of the
            // axis if it's inverted
            if (sensors[i]->isInverted())
                sensors[i]->setIdlePosition(0);
            else
                sensors[i]->setIdlePosition(255);
                
            // Enable calibration
            sensors[i]->passiveCalibrate(true);
        }
    }
    else
    {
        // Disabling calibration.  Set the flag to false.
        calibrating = false;

        // Disable calibration on the input axes
        for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        {
            // Disable calibration
            sensors[i]->passiveCalibrate(false);
            
            // Get the new sensor axis range and idle position
            sensors[i]->getRange(&axisMin, &axisMax);
            idlePos = sensors[i]->getIdlePosition();
            
            // Set the axis minimum to the same value as the idle position 
            // to eliminate any artifacts that might occur if the calibrated 
            // axis travels behind its idle position.
            sensors[i]->setRange(idlePos, axisMax);
        }
    }

    // Update all the buttons and axes
    vsIODevice::update();
}
