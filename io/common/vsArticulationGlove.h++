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
//    VESS Module:  vsArticulationGlove.h++
//
//    Description:  Device to keep track of the state of a VR articulation 
//                  glove.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_ARTICULATION_GLOVE_HPP
#define VS_ARTICULATION_GLOVE_HPP

// This class supports a glove device that measures hand articulation 
// (i.e.: the flexing of the fingers, thumb, and hand).  The hand is an
// extremely complex mechanism, and many sensors are needed to measure
// it with any degree of accuracy.  The standard VTI CyberGlove has 18
// sensors, and can optionally have as much as 22.
//
// This class was written with the VTI CyberGlove as a model, with hopes
// that other manufacturers' glove systems would fit this model.  The 
// CyberGlove's sensor arrangement is as follows:
//
// o Two sensors per digit measuring the metacarpophalangial and proximal 
//   interphalangial joints (MPJ and PIJ, the joint where the digit attaches 
//   to the palm and the next joint out toward the fingertip).  This is a 
//   total of ten sensors.
// o A sensor between each pair of digits (total of four) measuring the
//   abduction (angle) between the pair of digits.
// o Two additional sensors (one for the thumb and one for the pinky)
//   measuring how much it rotates across the palm toward the opposite
//   digit.
// o Two sensors measuring the pitch and yaw of the wrist.
//
// The CyberGlove can also optionally have an additional set of four sensors
// that measure the distal interphalangial joint (the joint nearest the tip)
// of each finger.
//
// This class endeavors to take the information from these sensors and
// calculate rotation values for a virtual hand model.  The model should
// have all the joints mentioned above (including the extra four distal
// interphalangial joints (DIJ)) as degrees of freedom.  If the DIJ 
// are not explicitly measured, their values will be estimated from the
// PIJ and MPJ joint values.
//
// The computed joint angles are stored and returned as vsQuat's and can
// be accessed with the getJoint() method.  The standard vsIODevice
// getAxis() and getButton() methods are present as well.  In this case,
// the getAxis() method returns the corresponding glove sensor data.
// the VS_AG_SENSOR_* symbols should be used to index the axes, and the
// VS_AG_JOINT_* symbols should be used to index the joints.

// Item counts
#define VS_AG_NUM_SENSORS 23
#define VS_AG_NUM_JOINTS  17
#define VS_AG_NUM_BUTTONS 1 

// Knuckle joint limits for all fingers
#define VS_AG_MPJ_LIMIT       90.0
#define VS_AG_PIJ_LIMIT      100.0
#define VS_AG_DIJ_LIMIT       70.0

// Abduction limits
#define VS_AG_INDEX_MIDDLE_ABD_LIMIT 30.0
#define VS_AG_MIDDLE_RING_ABD_LIMIT  30.0
#define VS_AG_RING_PINKY_ABD_LIMIT   45.0

// Limits for the thumb
#define VS_AG_THUMB_MJ_LIMIT  90.0
#define VS_AG_THUMB_MPJ_LIMIT 45.0
#define VS_AG_THUMB_IJ_LIMIT  70.0
#define VS_AG_THUMB_ABD_LIMIT 30.0

// Wrist scale factors and offsets
#define VS_AG_WRIST_FLEX_SCALE  140.0
#define VS_AG_WRIST_FLEX_OFFSET 40.0
#define VS_AG_WRIST_ABD_SCALE    40.0
#define VS_AG_WRIST_ABD_OFFSET   10.0

#include "vsIODevice.h++"
#include "vsQuat.h++"

// The glove sensors; use these for the getAxis() method.
enum VS_IO_DLL 
{
    VS_AG_SENSOR_THUMB_MJ,    // Thumb arch joint
    VS_AG_SENSOR_THUMB_MPJ,   // Thumb metacarpophalangial joint
    VS_AG_SENSOR_THUMB_IJ,    // Thumb interphalangial joint
    VS_AG_SENSOR_THUMB_ABD,   // Thumb abduction
    VS_AG_SENSOR_INDEX_MPJ,   // Index metacarpophalangial joint
    VS_AG_SENSOR_INDEX_PIJ,   // Index proximal interphalangial joint
    VS_AG_SENSOR_INDEX_DIJ,   // Index distal interphalangial joint
    VS_AG_SENSOR_INDEX_ABD,   // Index absolute abduction (not used)
    VS_AG_SENSOR_MIDDLE_MPJ,  // Middle metacarpophalangial joint
    VS_AG_SENSOR_MIDDLE_PIJ,  // Middle proximal interphalangial joint
    VS_AG_SENSOR_MIDDLE_DIJ,  // Middle distal interphalangial joint
    VS_AG_SENSOR_MIDDLE_ABD,  // Index/Middle relative abduction
    VS_AG_SENSOR_RING_MPJ,    // Ring metacarpophalangial joint
    VS_AG_SENSOR_RING_PIJ,    // Ring proximal interphalangial joint
    VS_AG_SENSOR_RING_DIJ,    // Ring distal interphalangial joint
    VS_AG_SENSOR_RING_ABD,    // Middle/Ring relative abduction
    VS_AG_SENSOR_PINKY_MPJ,   // Pinky metacarpophalangial joint
    VS_AG_SENSOR_PINKY_PIJ,   // Pinky proximal interphalangial joint
    VS_AG_SENSOR_PINKY_DIJ,   // Pinky distal interphalangial joint
    VS_AG_SENSOR_PINKY_ABD,   // Ring/Pinky relative abduction
    VS_AG_SENSOR_PALM_ARCH,   // Palm arch joint
    VS_AG_SENSOR_WRIST_PITCH, // Wrist pitch (flexion/extension)
    VS_AG_SENSOR_WRIST_YAW    // Wrist yaw (abduction/adduction)
};

// The actual joints; use these for the getJoint() method.
enum VS_IO_DLL 
{
     VS_AG_JOINT_THUMB_MJ,    // Thumb arch joint
     VS_AG_JOINT_THUMB_MPJ,   // Thumb metacarpophalangial joint
     VS_AG_JOINT_THUMB_IJ,    // Thumb interphalangial joint
     VS_AG_JOINT_INDEX_MPJ,   // Index metacarpophalangial joint
     VS_AG_JOINT_INDEX_PIJ,   // Index proximal interphalangial joint
     VS_AG_JOINT_INDEX_DIJ,   // Index distal interphalangial joint
     VS_AG_JOINT_MIDDLE_MPJ,  // Middle metacarpophalangial joint
     VS_AG_JOINT_MIDDLE_PIJ,  // Middle proximal interphalangial joint
     VS_AG_JOINT_MIDDLE_DIJ,  // Middle distal interphalangial joint
     VS_AG_JOINT_RING_MPJ,    // Ring metacarpophalangial joint
     VS_AG_JOINT_RING_PIJ,    // Ring proximal interphalangial joint
     VS_AG_JOINT_RING_DIJ,    // Ring distal interphalangial joint
     VS_AG_JOINT_PINKY_MPJ,   // Pinky metacarpophalangial joint
     VS_AG_JOINT_PINKY_PIJ,   // Pinky proximal interphalangial joint
     VS_AG_JOINT_PINKY_DIJ,   // Pinky distal interphalangial joint
     VS_AG_JOINT_PALM_ARCH,   // Palm arch joint
     VS_AG_JOINT_WRIST        // Wrist articulation
};

class VS_IO_DLL vsArticulationGlove : public vsIODevice
{
protected:

    // The final rotation values
    vsQuat           joints[VS_AG_NUM_JOINTS];

    // Indicates whether or not to estimate the distal interphalangial
    // joints
    bool             estimateDistal;

    // Indicates whether or not the glove is currently being calibrated
    bool             calibrating;

    // The raw sensor values
    vsInputAxis      *sensors[VS_AG_NUM_SENSORS];
    double           oldValue[VS_AG_NUM_SENSORS];

    // The CyberGlove actually has a button, other gloves may
    // have one, several, or none
    vsInputButton    *buttons[VS_AG_NUM_BUTTONS];

VS_INTERNAL:

    // Called to update the joint rotation from the current (new) 
    // value of the given axis (sensor)
    void             update();

public:
                     vsArticulationGlove(bool estDistal);
    virtual          ~vsArticulationGlove();

    // Inherited methods
    virtual const char    *getClassName();

    int              getNumAxes();
    int              getNumButtons();

    // NOTE: Requesting an axis returns the normalized glove sensor value
    //       and not the computed articulation of the joint
    vsInputAxis      *getAxis(int index);
    vsInputButton    *getButton(int index);

    // This method returns the computed articulation of the joint, which 
    // is probably more useful than the actual axis value
    vsQuat           getJoint(int index);

    // Load/Save calibration data
    void             saveCalibration(char *filename);
    void             loadCalibration(char *filename);

    // Enables/disables passive calibration
    void             passiveCalibrate(bool enable);
};

#endif
