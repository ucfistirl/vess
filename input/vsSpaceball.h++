#ifndef VS_SPACEBALL_HPP
#define VS_SPACEBALL_HPP

// A class for storing and returning the state of a spaceball

#include "vs6DInputDevice.h++"

#define VS_SB_MAX_BUTTONS 9

class vsSpaceball : public vs6DInputDevice
{
protected:

    // Number of buttons on spaceball
    int              numButtons;  
    vsInputButton    *button[VS_SB_MAX_BUTTONS];

VS_INTERNAL:

    void    setPosition(vsVector posVec);
    void    setOrientation(vsVector ornVec);
    void    setOrientation(vsMatrix ornMat);

public:

                             vsSpaceball(int nButtons);
    virtual                  ~vsSpaceball(void);

    virtual int              getNumButtons(void);
    virtual vsInputButton    *getButton(int index);
};

#endif
