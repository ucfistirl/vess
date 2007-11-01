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
//    VESS Module:  vsCyberGrasp.c++
//
//    Description:  Class to interface VESS applications with the
//                  Immersion 3D CyberGrasp force feedback system.
//                  This class makes use of the device proxy portion of 
//                  Immersion's Virtual Hand Toolkit library, so this must 
//                  be installed and linked with any application that makes
//                  use of this class.  NOTE: this does NOT require the full
//                  commercial Virtual Hand Toolkit, only the device proxys
//                  which are available for free.
//
//                  The hardware should be configured and calibrated
//                  with Immersion's Device Configuration Utility (DCU)
//                  before attempting to use this class.  The registry.vrg
//                  file created by the DCU must be available to the
//                  application (usually by setting the VTI_REGISTRY_FILE
//                  environment variable).
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsCyberGrasp.h++"
#include "vhandtk/vhtTransform3D.h"
#include "vhandtk/vhtVector3d.h"
#include "vhandtk/vhtQuaternion.h"
#include "vhandtk/vhtContactPatch.h"

// ------------------------------------------------------------------------
// Constructs a vsCyberGrasp using only the VTi registry file (which must
// be set using the VTI_REGISTRY_FILE environment variable).  The 
// CyberGrasp must be properly configured using the Device Configuration 
// Utility and must include the CyberGlove and motion tracker.
// ------------------------------------------------------------------------
vsCyberGrasp::vsCyberGrasp()
{
    // Set up I/O connections to the devices
    trackerConn = vhtIOConn::getDefault(vhtIOConn::tracker);
    gloveConn = vhtIOConn::getDefault(vhtIOConn::glove);
    graspConn = vhtIOConn::getDefault(vhtIOConn::grasp);

    // Create the device objects
    vhtTrackerSys = new vhtTracker(trackerConn);
    vhtTrackerObject = vhtTrackerSys->getLogicalDevice(0);
    vhtTrackerEmu = NULL;
    vhtGlove = new vhtCyberGlove(gloveConn);
    vhtGrasp = new vhtCyberGrasp(graspConn, gloveConn);

    // Create a vsMotionTracker for the tracker attached to the
    // CyberGrasp
    tracker = new vsMotionTracker();
    
    // Create a coordinate system transform that translates CyberGrasp
    // coordinates to VESS coordinates.  CyberGrasp coordinates are
    // equivalent to OpenGL (+X right, +Y up, +Z out from the screen)
    coordXform.setAxisAngleRotation(1, 0, 0, 90);
    coordXformInv = coordXform.getInverse();

    // Flag that we're using a tracker controlled by the CyberGrasp FCU
    localTracker = false;

    // Initialize the rest of the object
    initialize();
}

// ------------------------------------------------------------------------
// Constructs a vsCyberGrasp using the VHT (which must be set up using the
// VTI_REGISTRY_FILE environment variable), along with a locally-
// controlled motion tracker.  The CyberGrasp must be properly configured 
// using the Device Configuration Utility and must include the CyberGlove.
// The motion tracker is handled on the external hardware through the use 
// of a vhtTrackerEmulator object.
// ------------------------------------------------------------------------
vsCyberGrasp::vsCyberGrasp(vsMotionTracker *trkr)
{
    // Set up I/O connections to the devices
    gloveConn = vhtIOConn::getDefault(vhtIOConn::glove);
    graspConn = vhtIOConn::getDefault(vhtIOConn::grasp);

    // Create the device objects
    vhtGlove = new vhtCyberGlove(gloveConn);
    vhtGrasp = new vhtCyberGrasp(graspConn, gloveConn);

    // Save the motion tracker
    tracker = trkr;
    vhtTrackerSys = NULL;
    vhtTrackerObject = NULL;
    vhtTrackerEmu = new vhtTrackerEmulator();
    
    // Flag that we're using a locally controlled tracker
    localTracker = false;

    // Initialize the rest of the object
    initialize();
}

// ------------------------------------------------------------------------
// Destructor.  Cleans up the objects created
// ------------------------------------------------------------------------
vsCyberGrasp::~vsCyberGrasp()
{
    // Put the CyberGrasp in REWIND mode
    if (vhtGrasp)
    {
        setFeedbackMode(VS_CGR_MODE_REWIND);
    }
    
    // Delete VHT object
    if (vhtGlove)
        delete vhtGlove;
    if (vhtGrasp)
        delete vhtGrasp;
    if (vhtTrackerSys)
        delete vhtTrackerSys;
    if (vhtTrackerObject)
        delete vhtTrackerObject;
    if (vhtTrackerEmu)
        delete vhtTrackerEmu;
        
    // Delete the vsArticulationGlove
    if (glove)
        delete glove;
}

// ------------------------------------------------------------------------
// Performs common initialization for both of the class's constructors
// ------------------------------------------------------------------------
void vsCyberGrasp::initialize()
{
    // Create the glove device object (joint articulations are stored here)
    glove = new vsArticulationGlove(false);

    // Invert the finger abduction axes on the articulation glove, as they 
    // need to cause a smaller amount of abduction as the sensor reading 
    // increases
    glove->getAxis(VS_AG_SENSOR_MIDDLE_ABD)->setInverted(true);
    glove->getAxis(VS_AG_SENSOR_RING_ABD)->setInverted(true);
    glove->getAxis(VS_AG_SENSOR_PINKY_ABD)->setInverted(true);

    // Initialize forces to zero
    memset(forces, 0, sizeof(forces));
    
    // Set the grasp to REWIND mode
    setFeedbackMode(VS_CGR_MODE_REWIND);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCyberGrasp::getClassName()
{
    return "vsCyberGrasp";
}

// ------------------------------------------------------------------------
// Return the vsArticulationGlove created by this object
// ------------------------------------------------------------------------
vsArticulationGlove *vsCyberGrasp::getGlove()
{
    return glove;
}

// ------------------------------------------------------------------------
// Return the vsMotionTracker created by this object (or the
// vsMotionTracker passed in if using a tracking system not attached to
// the CyberGrasp CGIU)
// ------------------------------------------------------------------------
vsMotionTracker *vsCyberGrasp::getTracker()
{
    return tracker;
}

// ------------------------------------------------------------------------
// Sets the current feedback mode (IMPEDANCE, FORCE, REWIND, or IDLE)  This
// determines how the haptic feedback on the CyberGrasp is to be
// controlled.
// ------------------------------------------------------------------------
void vsCyberGrasp::setFeedbackMode(int mode)
{
    // Call the VHT method to change the feedback mode
    vhtGrasp->setMode(mode);
}

// ------------------------------------------------------------------------
// Returns the current feedback mode (IMPEDANCE, FORCE, REWIND, or IDLE)
// ------------------------------------------------------------------------
int vsCyberGrasp::getFeedbackMode()
{
    // Call the VHT method to query the feedback mode
    return vhtGrasp->getMode();
}

// ------------------------------------------------------------------------
// Sets the applied force on a given finger to the specified value (must be
// in FORCE mode)
// ------------------------------------------------------------------------
void vsCyberGrasp::setForce(int finger, double force)
{
    // Validate the finger parameter
    if ((finger < VS_CGR_FINGER_THUMB) || (force > VS_CGR_FINGER_PINKY))
    {
        printf("vsCyberGrasp::setForce:  Invalid finger specified\n");
        return;
    }
    
    // Validate the force parameter
    if ((force < 0.0) || (force > 1.0))
    {
        printf("vsCyberGrasp::setForce:  Invalid force value\n");
        return;
    }
    
    // Change the entry in the forces array
    forces[finger] = force;
    
    // Set the new forces on the CyberGrasp
    vhtGrasp->setForce(forces);
}

// ------------------------------------------------------------------------
// Sets the applied force on all fingers to the specified values (must be
// in FORCE mode).  The force parameter should be an array of 5 double
// values.
// ------------------------------------------------------------------------
void vsCyberGrasp::setForces(double *newForces)
{
    int i;
    
    // Validate the force parameters
    for (i = 0; i < 5; i++)
    {
        if ((newForces[i] < 0.0) || (newForces[i] > 1.0))
        {
            printf("vsCyberGrasp::setForce:  Invalid force value specified\n");
            return;
        }
    }
    
    // Copy the new forces to the forces array
    memcpy(forces, newForces, 5 * sizeof(double));
    
    // Set the new forces on the CyberGrasp
    vhtGrasp->setForce(forces);
}

// ------------------------------------------------------------------------
// Sets a contact patch (a plane of intersection) on the CyberGrasp (must 
// be in IMPEDANCE mode).  The CyberGrasp will calculate the force
// necessary to keep the finger from traveling through the plane of the
// contact patch.
// ------------------------------------------------------------------------
void vsCyberGrasp::setContactPatch(int finger, atVector point, 
                                   atVector normal, double stiffness, 
                                   double damping)
{
    vhtVector3d vhtOffset, vhtNormal;
    vhtContactPatch vhtPatch;
    
    // Validate the finger parameter
    if ((finger < VS_CGR_FINGER_THUMB) || (finger > VS_CGR_FINGER_PINKY))
    {
        printf("vsCyberGrasp::setContactPatch:  Invalid finger specified\n");
        return;
    }

    // Validate the stiffness parameter
    if ((stiffness < 0.0) || (stiffness > 1.0))
    {
        printf("vsCyberGrasp::setContactPatch:  Invalid stiffness value,"
            " assuming 1.0\n");
            
        stiffness = 1.0;
    }

    // Validate the damping parameter
    if ((stiffness < 0.0) || (stiffness > 1.0))
    {
        printf("vsCyberGrasp::setContactPatch:  Invalid damping value,"
            " assuming 1.0\n");
            
        damping = 1.0;
    }
    
    // Transform the patch coordinates into VESS coordinates
    //point = coordXformInv.rotatePoint(point);
    //normal = coordXformInv.rotatePoint(normal);
    
    // Get the parameters into VHT format
    vhtOffset = vhtVector3d(point[AT_X], point[AT_Y], point[AT_Z]);
    vhtNormal = vhtVector3d(normal[AT_X], normal[AT_Y], normal[AT_Z]);
    
    // Set up the contact patch
    vhtPatch.setStiffness(stiffness);
    vhtPatch.setDamping(damping);
    vhtPatch.setOffset(vhtOffset);
    vhtPatch.setNormal(vhtNormal);
    vhtPatch.setPatchFrame(vhtContactPatch::world);
    
    // Apply the patch
    vhtGrasp->setContactPatch(finger, &vhtPatch);
}

// ------------------------------------------------------------------------
// Removes the contact patch on the given joint (must be in IMPEDANCE mode)
// ------------------------------------------------------------------------
void vsCyberGrasp::clearContactPatch(int finger)
{
    // Validate the finger parameter
    if ((finger < VS_CGR_FINGER_THUMB) || (finger > VS_CGR_FINGER_PINKY))
    {
        printf("vsCyberGrasp::clearContactPatch:  Invalid finger specified\n");
        return;
    }
    
    // Reset the finger's contact patch
    vhtGrasp->resetContactPatch(finger);
}

// ------------------------------------------------------------------------
// Updates the CyberGrasp
// ------------------------------------------------------------------------
void vsCyberGrasp::update()
{
    atVector position, orientation;
    double xRot, yRot, zRot;
    vhtTransform3D trackerXform;
    vhtVector3d vhtVec;
    vhtQuaternion vhtQuat;
    atVector trackerVec;
    atQuat trackerQuat;
    atQuat quat1, quat2, localCoordXform;
    double angle;
    int i, j;
    double sensorData;
    int axisIndex;
    vsInputAxis *axis;

    // Update the tracker if we're not using a locally controlled tracker
    if (!localTracker)
        vhtTrackerObject->update();

    // Update the glove and grasp devices
    vhtGlove->update();
    vhtGrasp->update();
   
    // If using a local tracker, update the emulator's data now
    if (localTracker)
    {
        position = tracker->getPositionVec();
        orientation = tracker->getOrientationVec(AT_EULER_ANGLES_ZXY_R);

        // Set the tracker emulator's position
        vhtTrackerEmu->setTrackerPosition(position[AT_X], position[AT_Y], 
            position[AT_Z]);

        // Set the orientation.  The setOrientation method in the
        // vhtTrackerEmulator class takes x, y, and z rotations in 
        // radians, which corresponds to the pitch, roll, and heading
        // of the tracker, respectively.  First, compute the rotation 
        // values:
        xRot = AT_DEG2RAD(orientation[VS_P]);
        yRot = AT_DEG2RAD(orientation[VS_R]);
        zRot = AT_DEG2RAD(orientation[VS_H]);

        // Now, set the tracker's orientation
        vhtTrackerEmu->setTrackerOrientation(xRot, yRot, zRot);
    }
    else
    {
        // Not using a local tracker, update the vsMotionTracker with
        // fresh data from the CyberGrasp
        
        // First, get the current tracker measurements
        vhtTrackerObject->getTransform(&trackerXform);
        trackerXform.getTranslation(vhtVec);
        trackerXform.getRotation(vhtQuat);

        // Set the vsMotionTracker's position
        trackerVec.set(vhtVec.x, vhtVec.y, vhtVec.z);
        //trackerVec = coordXform.rotatePoint(trackerVec);
        tracker->setPosition(trackerVec);

        // Set the vsMotionTracker's orientation
        vhtQuat.getAxis(vhtVec);
        angle = AT_RAD2DEG(vhtQuat.getAngle());
        trackerQuat.setAxisAngleRotation(vhtVec.x, vhtVec.y, vhtVec.z, angle);
        //quat1.setAxisAngleRotation(1, 0, 0, -90);
        //quat2.setAxisAngleRotation(0, 1, 0, -90);
        //localCoordXform = quat2 * quat1;
        //trackerQuat = coordXform * trackerQuat * coordXformInv *
        //    localCoordXform;
        tracker->setOrientation(trackerQuat);
    }

    // Get the state of the hand and update the vsArticulationGlove
    // sensors
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 4; j++)
        {
            // Translate the finger i, joint j value of the VHT generic 
            // hand model to a vsArticulationGlove sensor index
            // (essentially, map the VHT sensor axis to a VESS sensor axis)
            axisIndex = vsCyberGraspFingerJointToSensorMap[i][j];
            
            // Get the vsArticulationGlove axis that we mapped above.  If
            // there is no valid mapping, skip this axis.
            if (axisIndex >= 0)
            {           
                axis = glove->getAxis(axisIndex);
            
                // Get the sensor data from the corresponding CyberGlove 
                // sensor
                sensorData = vhtGlove->getRawData(
                    (GHM::Fingers)i, (GHM::Joints)j);
            
                // Set the sensor axis's new position
                axis->setPosition(sensorData);
            }
        }
    }
    
    // Update the vsArticulationGlove
    glove->update();
}
