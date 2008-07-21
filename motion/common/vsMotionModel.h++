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
//    VESS Module:  vsMotionModel.h++
//
//    Description:  Abstract base class for all motion models
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_MOTION_MODEL_HPP
#define VS_MOTION_MODEL_HPP

#include "vsUpdatable.h++"

class VESS_SYM vsMotionModel : public vsUpdatable
{
public:

    // Constructor/destructor
                        vsMotionModel();
    virtual             ~vsMotionModel();

    // Update function.  Must be overridden by any concrete descendant.
    virtual void        update() = 0;

    // Reset (calibration) function.  May be overridden, not required.
    virtual void        reset();
};

#endif
