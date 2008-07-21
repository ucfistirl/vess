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
//    VESS Module:  vsInputAxis.h++
//
//    Description:  Class for handling the position of an input device's
//                  axis
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_INPUT_AXIS_HPP
#define VS_INPUT_AXIS_HPP

// Values are stored as raw device values (the values returned by the
// hardware).  When retrieved, they are converted to a scaled value
// within the range of -1.0 to 1.0.

#include "vsGlobals.h++"
#include "vsUpdatable.h++"

#define VS_AXIS_DEFAULT_MIN 0.0
#define VS_AXIS_DEFAULT_MAX 255.0

class VESS_SYM vsInputAxis : public vsUpdatable
{
protected:

    // Position of the axis (raw device value)
    double       position;

    // Position of the axis (raw device value) at the last 2 update() calls
    // These are used to calculate relative positions
    double       previousPosition1;    
    double       previousPosition2;

    // Calibration offset (raw device value)
    double       offset;   

    // Indicates whether or not to normalize the axis value
    bool         normalized;

    // Indicates whether or not to invert the axis value
    bool         inverted;

    // Minimum and maximum positions (raw device values)
    double       axisMin;
    double       axisMax;

    // Threshold value (axis values below this are reported as zero)
    double       threshold;

    // Indicates whether or not passive calibration is enabled
    bool         passiveCalibration;

    // Returns the normalized value of the given raw value
    double       getNormalizedValue(double rawValue);

VS_INTERNAL:

    void            setPosition(double rawPos);

    // Normally, the previous position is automatically saved by setPosition(),
    // these are for special instances when you need to override that value
    // (i.e. the mouse wrapping in vsWindowSystem uses this)
    void            forcePreviousPosition(double rawPos);
    void            forceShiftPreviousPosition(double rawShiftPos);

    virtual void    update();

public:

                            vsInputAxis();
                            vsInputAxis(double minPos, double maxPos);
    virtual                 ~vsInputAxis();

    virtual const char *    getClassName();

    double                  getPosition();
    double                  getDelta();

    // Axis operations
    void                    setNormalized(bool normOn);
    bool                    isNormalized();
    void                    setInverted(bool invert);
    bool                    isInverted();
    void                    setRange(double minPos, double maxPos);
    void                    getRange(double *minPos, double *maxPos);

    // Calibration functions
    void                    setIdlePosition();
    void                    setIdlePosition(double newOffset);
    double                  getIdlePosition();

    // Threshold functions
    void                    setThreshold(double newThreshold);
    double                  getThreshold();

    void                    passiveCalibrate(bool enable);
};

#endif
