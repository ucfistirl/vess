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
// should be estimated or not.  VS_TRUE means yes, VS_FALSE means no.
// ------------------------------------------------------------------------
vsArticulationGlove::vsArticulationGlove(int estDistal)
{
    int i;

    estimateDistal = estDistal;
    calibrating = VS_FALSE;

    // Start with a reasonable range
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        sensors[i] = new vsInputAxis(1, 255);

    for (i = 0; i < VS_AG_NUM_BUTTONS; i++)
        buttons[i] = new vsInputButton();
}

// ------------------------------------------------------------------------
// Destructor.
// ------------------------------------------------------------------------
vsArticulationGlove::~vsArticulationGlove()
{
    int i;

    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        delete sensors[i];

    for (i = 0; i < VS_AG_NUM_BUTTONS; i++)
        delete buttons[i];
}

// ------------------------------------------------------------------------
// Updates the joint angles from the current sensor values.  Call this 
// method after updating all the sensor values.
// ------------------------------------------------------------------------
void vsArticulationGlove::update()
{
    int    i;
    double deg1, deg2;
    vsQuat quat1, quat2;

    // If we're calibrating, see if we need to update any idle positions
    // If not, make sure the axis values don't go negative
    for (i = 0; i < VS_AG_NUM_SENSORS; i++)
    {
        if (sensors[i]->getPosition() < 0.0)
        {
            if (calibrating)
            {
                // Change the idle position to the current sensor position if
                // the current position is less than the current idle position
                sensors[i]->setIdlePosition();
            }
            else
            {
                sensors[i]->setPosition(sensors[i]->getIdlePosition());
            }
        }
    }

    // Read the thumb MJ sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_MJ]->getPosition() * 90.0;

    // Compute the joint angle
    joints[VS_AG_JOINT_THUMB_MJ].setAxisAngleRotation(0, 1, 0, 90.0);
    

    // Read the thumb MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_THUMB_MPJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the thumb IJ sensor
    deg1 = sensors[VS_AG_SENSOR_THUMB_IJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_THUMB_IJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the index MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_INDEX_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_INDEX_MPJ].setAxisAngleRotation(1, 0, 0, deg1);
 
    // Read the index PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_INDEX_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_INDEX_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

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

    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_MIDDLE_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the ring MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_RING_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_RING_MPJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the ring PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_RING_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_RING_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

    if (estimateDistal)
    {
        // Average the two values
        deg1 = (deg1 + deg2) / 2.0;

        // Compute the distal joint angle
        joints[VS_AG_JOINT_RING_DIJ].setAxisAngleRotation(1, 0, 0, deg1);
    }

    // Read the MPJ sensor
    deg1 = sensors[VS_AG_SENSOR_PINKY_MPJ]->getPosition() * VS_AG_MPJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_PINKY_MPJ].setAxisAngleRotation(1, 0, 0, deg1);

    // Read the PIJ sensor
    deg2 = sensors[VS_AG_SENSOR_PINKY_PIJ]->getPosition() * VS_AG_PIJ_LIMIT;

    // Compute the joint angle
    joints[VS_AG_JOINT_PINKY_PIJ].setAxisAngleRotation(1, 0, 0, deg2);

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

    // Read the wrist sensors
    deg1 = sensors[VS_AG_SENSOR_WRIST_PITCH]->getPosition() * -140.0 + 100.0;
    deg2 = sensors[VS_AG_SENSOR_WRIST_YAW]->getPosition() * -40.0 + 10.0;

    // Compute the joint angle
    quat1.setAxisAngleRotation(1, 0, 0, deg1);
    quat2.setAxisAngleRotation(0, 0, 1, deg2);
    joints[VS_AG_JOINT_WRIST] = quat2 * quat1;

    // Palm Arch (not yet)
    joints[VS_AG_JOINT_PALM_ARCH].setAxisAngleRotation(0, 0, 0, 1);

    // Update the abduction values
    // (not yet)
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
    if ((index >= 0) && (index < VS_AG_NUM_BUTTONS))
        return buttons[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Returns the vsQuat corresponding to the given joint index
// ------------------------------------------------------------------------
vsQuat vsArticulationGlove::getJoint(int index)
{
    if ((index >= 0) && (index < VS_AG_NUM_JOINTS))
        return joints[index];
    else
        return NULL;
}

// ------------------------------------------------------------------------
// Enables/disables passive calibration for all sensors
// ------------------------------------------------------------------------
void vsArticulationGlove::passiveCalibrate(int enable)
{
    int i;

    if (enable)
    {
        calibrating = VS_TRUE;
        for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        {
            sensors[i]->setIdlePosition(255);
            sensors[i]->passiveCalibrate(VS_TRUE);
        }
    }
    else
    {
        calibrating = VS_FALSE;
        for (i = 0; i < VS_AG_NUM_SENSORS; i++)
        {
            sensors[i]->passiveCalibrate(VS_FALSE);
        }
    }
}
