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
//    Description:  Interface for a VTI CyberGlove articulation glove
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_CYBER_GLOVE_BOX_HPP
#define VS_CYBER_GLOVE_BOX_HPP

// This class supports the VTI CyberGlove.  It makes use of a 
// vsArticulationGlove object to keep track of its state.

#include "vsArticulationGlove.h++"
#include "vsSerialPort.h++"
#include "vsInputSystem.h++"

// CyberGlove Commands (using VS_CYG_* instead of VS_CG, which is taken by
// vsChordGloves).  All commands here use binary mode.  For ASCII mode, use
// the same letter, only lower-case.
#define VS_CYG_CMD_PING           'G'
#define VS_CYG_CMD_STREAM         'S'

// The query command
#define VS_CYG_CMD_QUERY          '?'

// These commands are queryable use the query command before any of the
// following commands to get the current setting for that command
#define VS_CYG_CMD_BAUD           'B'
#define VS_CYG_CMD_SENSOR_MASK    'M'
#define VS_CYG_CMD_NUM_SENSORS    'N'
#define VS_CYG_CMD_PARAM_FLAGS    'P'
#define VS_CYG_CMD_SAMPLE_PERIOD  'T'
#define VS_CYG_CMD_CYBERTOUCH     'A'

// Query-only commands, must be preceded with the query command '?'
#define VS_CYG_CMD_GLOVE_STATUS   'G'
#define VS_CYG_CMD_GLOVE_INFO     'i' // (ASCII only)
#define VS_CYG_CMD_HW_MASK        'K'
#define VS_CYG_CMD_RIGHT_HANDED   'R'
#define VS_CYG_CMD_NUM_HW_SENSORS 'S'
#define VS_CYG_CMD_VERSION        'V'

// Parameter flags (set with the VS_CYG_CMD_PARAM_FLAGS) command
//   Byte 1
#define VS_CYG_PARAM_GLOVE_INOUT       0x01
#define VS_CYG_PARAM_SWITCH_ON         0x02
#define VS_CYG_PARAM_LIGHT_ON          0x04
//   Byte 2
#define VS_CYG_PARAM_BINARY_SYNC       0x01
#define VS_CYG_PARAM_ASCII_SYNC        0x02
#define VS_CYG_PARAM_INC_STATUS        0x04
#define VS_CYG_PARAM_SWITCH_CTRL_LIGHT 0x08
#define VS_CYG_PARAM_DIGITAL_FILTER    0x10
#define VS_CYG_PARAM_INC_TIMESTAMP     0x20
#define VS_CYG_PARAM_GLOVE_HAND        0x40
#define VS_CYG_PARAM_GLOVE_VALID       0x80
//  Byte 3
#define VS_CYG_PARAM_QUANTIZE          0x01
#define VS_CYG_PARAM_CYBERTOUCH        0x02

// CyberTouch actuator indices
#define VS_CYG_NUM_ACTUATORS 6
enum
{
    VS_CYG_TOUCH_THUMB  = 0,
    VS_CYG_TOUCH_INDEX  = 1,
    VS_CYG_TOUCH_MIDDLE = 2,
    VS_CYG_TOUCH_RING   = 3,
    VS_CYG_TOUCH_PINKY  = 4,
    VS_CYG_TOUCH_PALM   = 5
};

class vsCyberGloveBox : public vsInputSystem
{
protected:

    vsSerialPort           *port;

    vsArticulationGlove    *glove;
    int                    numSensors;
    int                    touchInstalled;

    // Initializes the glove box
    void                   initialize();

    // Ping the box for a new sensor sample
    void                   ping();

public:

                           vsCyberGloveBox(int portNum, long baud, 
                                           int nSensors);
                           ~vsCyberGloveBox();

    // Inherited from vsObject
    virtual const char     *getClassName();

    // Glove accessor
    vsArticulationGlove    *getGlove();

    // CyberTouch functions
    void                   startFeedback(int index, int amplitude);
    void                   stopFeedback(int index);
    void                   startAllFeedback(int amplitude);
    void                   stopAllFeedback();

    // The update function
    void                   update();
};

#endif
