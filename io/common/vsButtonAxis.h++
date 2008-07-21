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
//    VESS Module:  vsButtonAxis.h++
//
//    Description:  Class for emulating the behavior of an input axis
//                  based on the input of some number of vsInputButtons
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_BUTTON_AXIS_HPP
#define VS_BUTTON_AXIS_HPP

#include "vsIODevice.h++"

class VESS_SYM vsButtonAxis : public vsIODevice
{
private:

    // Input buttons
    vsInputButton    *positiveButton;
    vsInputButton    *negativeButton;
    vsInputButton    *centerButton;

    // Output axis
    vsInputAxis      *outputAxis;

    // Speed (in units/sec) of position movement along the axis
    double           positiveSpeed;
    double           negativeSpeed;
    double           centerSpeed;
    double           idleSpeed;

    // Current axis position
    double           position;
    void             setPosition(double newPos);

public:

    // Constructors and destructor
                             vsButtonAxis(vsInputButton *positiveBtn,
                                          vsInputButton *negativeBtn,
                                          vsInputButton *centerBtn);
                             vsButtonAxis(vsInputButton *positiveBtn,
                                          vsInputButton *negativeBtn,
                                          vsInputButton *centerBtn,
                                          double axisMin, double axisMax);
    virtual                  ~vsButtonAxis();

    // Inherited from vsObject
    virtual const char       *getClassName();

    // Inherited from vsUpdatable
    virtual void             update();

    // Inherited from vsIODevice
    virtual int              getNumAxes();
    virtual int              getNumButtons();

    virtual vsInputAxis      *getAxis(int index);
    virtual vsInputButton    *getButton(int index);

    // Speed setting/getting functions
    void                     setPositiveButtonSpeed(double speed);
    double                   getPositiveButtonSpeed();
    void                     setNegativeButtonSpeed(double speed);
    double                   getNegativeButtonSpeed();
    void                     setCenterButtonSpeed(double speed);
    double                   getCenterButtonSpeed();
    void                     setIdleSpeed(double speed);
    double                   getIdleSpeed();
};

#endif
