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
//    VESS Module:  vsCyberGrasp.h++
//
//    Description:  Class to interface VESS applications with the
//                  Immersion 3D CyberGrasp force feedback system.
//                  This class makes use of Immersion's Virtual Hand 
//                  Toolkit library, so this must be installed and linked
//                  with any application that makes use of this class.
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

#ifndef VS_CYBER_GRASP_HPP
#define VS_CYBER_GRASP_HPP

#include <vhandtk/vhtCyberGrasp.h>
#include <vhandtk/vhtCyberGlove.h>
#include <vhandtk/vhtTracker.h>
#include <vhandtk/vhtTrackerEmulator.h>
#include <vhandtk/vhtIOConn.h>
#include "vsArticulationGlove.h++"
#include "vsMotionTracker.h++"
#include "vsIOSystem.h++"

// Force control modes
//   NOTE: the "IMPEDENCE" token below is misspelled in the VHT libraries, 
//   so it is mispelled on purpose here
#define VS_CGR_MODE_FORCE     GR_CONTROL_FORCE
#define VS_CGR_MODE_IMPEDANCE GR_CONTROL_IMPEDENCE
#define VS_CGR_MODE_REWIND    GR_CONTROL_REWIND
#define VS_CGR_MODE_IDLE      GR_CONTROL_IDLE

// Finger indices
#define VS_CGR_FINGER_THUMB  GHM::thumb
#define VS_CGR_FINGER_INDEX  GHM::index
#define VS_CGR_FINGER_MIDDLE GHM::middle
#define VS_CGR_FINGER_RING   GHM::ring
#define VS_CGR_FINGER_PINKY  GHM::pinky

// Mapping from vsArticulationGlove sensors to vhtGenHandModel finger,joint
// pairs
const int vsCyberGraspSensorToFingerJointMap[][2] =
{
    {GHM::thumb, GHM::metacarpal},
    {GHM::thumb, GHM::proximal},
    {GHM::thumb, GHM::distal},
    {GHM::thumb, GHM::abduct},
    {GHM::index, GHM::metacarpal},
    {GHM::index, GHM::proximal},
    {GHM::index, GHM::distal},
    {GHM::index, GHM::abduct},
    {GHM::middle, GHM::metacarpal},
    {GHM::middle, GHM::proximal},
    {GHM::middle, GHM::distal},
    {GHM::middle, GHM::abduct},
    {GHM::ring, GHM::metacarpal},
    {GHM::ring, GHM::proximal},
    {GHM::ring, GHM::distal},
    {GHM::ring, GHM::abduct},
    {GHM::pinky, GHM::metacarpal},
    {GHM::pinky, GHM::proximal},
    {GHM::pinky, GHM::distal},
    {GHM::pinky, GHM::abduct},
    {GHM::palm, GHM::palmArch},
    {GHM::palm, GHM::wristFlexion},
    {GHM::palm, GHM::wristAbduction}
};

// Mapping from vhtGenHandModel finger,joint pairs to vsArticulationGlove 
// sensors
const int vsCyberGraspFingerJointToSensorMap[6][4] =
{
    {VS_AG_SENSOR_THUMB_MJ, VS_AG_SENSOR_THUMB_MPJ, VS_AG_SENSOR_THUMB_IJ,
        VS_AG_SENSOR_THUMB_ABD},
    {VS_AG_SENSOR_INDEX_MPJ, VS_AG_SENSOR_INDEX_PIJ, VS_AG_SENSOR_INDEX_DIJ,
        VS_AG_SENSOR_INDEX_ABD},
    {VS_AG_SENSOR_MIDDLE_MPJ, VS_AG_SENSOR_MIDDLE_PIJ, VS_AG_SENSOR_MIDDLE_DIJ,
        VS_AG_SENSOR_MIDDLE_ABD},
    {VS_AG_SENSOR_RING_MPJ, VS_AG_SENSOR_RING_PIJ, VS_AG_SENSOR_RING_DIJ,
        VS_AG_SENSOR_RING_ABD},
    {VS_AG_SENSOR_PINKY_MPJ, VS_AG_SENSOR_PINKY_PIJ, VS_AG_SENSOR_PINKY_DIJ,
        VS_AG_SENSOR_PINKY_ABD},
    {VS_AG_SENSOR_PALM_ARCH, VS_AG_SENSOR_WRIST_PITCH, VS_AG_SENSOR_WRIST_YAW,
        -1},
};

class VS_IO_DLL vsCyberGrasp : public vsIOSystem
{
protected:

    // I/O Connections for the various devices
    vhtIOConn              *graspConn;
    vhtIOConn              *gloveConn;
    vhtIOConn              *trackerConn;

    // VHT device interfaces
    vhtCyberGrasp          *vhtGrasp;
    vhtCyberGlove          *vhtGlove;
    vhtTracker             *vhtTrackerSys;
    vht6DofDevice          *vhtTrackerObject;

    // Classes for use with a locally-controlled tracker
    vsMotionTracker        *tracker;
    vhtTrackerEmulator     *vhtTrackerEmu;
    
    // Coordinate system transforms
    vsQuat                 coordXform, coordXformInv;

    // Indicates whether or not we're using a tracker controlled locally
    // or one controlled by the CyberGrasp FCU
    int                    localTracker;
    
    // Current forces applied by the CyberGrasp (FORCE mode only)
    double                 forces[5];

    // Glove representation used in intersection tests
    vsArticulationGlove    *glove;

    // Initializes the CyberGrasp
    void                   initialize();

public:

    // Constructs a vsCyberGrasp with the motion tracker controlled by
    // the CyberGrasp system
                           vsCyberGrasp();

    // Constructs a vsCyberGrasp with the motion tracker controlled by
    // the local application
                           vsCyberGrasp(vsMotionTracker *trkr);
                           
    // Destructor
                           ~vsCyberGrasp();
                           
    // Return the class name
    const char             *vsCyberGrasp::getClassName();

    // Glove accessor
    vsArticulationGlove    *getGlove();
    
    // Tracker accessor
    vsMotionTracker        *getTracker();

    // Sets the current feedback mode (IMPEDANCE, FORCE, REWIND, or IDLE)
    void                   setFeedbackMode(int mode);
    int                    getFeedbackMode();

    // Sets the applied force on a given finger (or all fingers) to the 
    // specified value(s) (must be in FORCE mode)
    void                   setForce(int finger, double force);
    void                   setForces(double *force);

    // Sets a contact patch (a plane of intersection) on the CyberGrasp
    // (must be in IMPEDANCE mode)
    void                   setContactPatch(int finger, vsVector point, 
                                           vsVector normal, double stiffness, 
                                           double damping);

    // Removes the contact patch on the given joint (must be in IMPEDANCE 
    // mode)
    void                   clearContactPatch(int finger);

    // Updates the system
    virtual void    update();
};

#endif
