#ifndef VS_INPUT_DEVICE_HPP
#define VS_INPUT_DEVICE_HPP

// Abstract base class for all VESS input devices

#include "vsGlobals.h++"
#include "vsInputAxis.h++"
#include "vsInputButton.h++"

class vsInputDevice 
{
public:

                             vsInputDevice();
    virtual                  ~vsInputDevice();

    virtual int              getNumAxes() = 0;
    virtual int              getNumButtons() = 0;

    virtual vsInputAxis      *getAxis(int index) = 0;
    virtual vsInputButton    *getButton(int index) = 0;
};

#endif
