#ifndef VS_INPUT_AXIS_HPP
#define VS_INPUT_AXIS_HPP

// Class for handling the position of an input device's axis
//
// Values are stored as raw device values (the values returned by the
// hardware).  When retrieved, they are converted to a scaled value
// within the range of -1.0 to 1.0.

#include "vsGlobals.h++"

#define VS_AXIS_DEFAULT_MIN 0.0
#define VS_AXIS_DEFAULT_MAX 255.0

class vsInputAxis
{
protected:

    // Position of the axis (raw device value)
    double       position;    

    // Calibration offset (raw device value)
    double       offset;   

    // Indicates whether or not to normalize the axis value
    int          normalized;

    // Minimum and maximum positions (raw device values)
    double       axisMin;
    double       axisMax;

    // Indicates whether or not passive calibration is enabled
    int          passiveCalibration;

VS_INTERNAL:

    void         setPosition(double rawPos);

public:

                 vsInputAxis();
                 vsInputAxis(double minPos, double maxPos);
                 ~vsInputAxis();

    double       getPosition();

    // Axis operations
    void         setNormalized(int normalize);
    int          isNormalized();
    void         setRange(double minPos, double maxPos);
    void         getRange(double *minPos, double *maxPos);

    // Calibration functions
    void         setIdlePosition();
    void         setIdlePosition(double newOffset);

    void         passiveCalibrate(int enable);
};

#endif
