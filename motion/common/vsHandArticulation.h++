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
//    VESS Module:  vsHandArticulation.h++
//
//    Description:  A class to allow the 22 degrees of freedom of the hand
//                  to be manipulated and updated as a unit.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_HAND_ARTICULATION_HPP
#define VS_HAND_ARTICULATION_HPP

#include "vsMotionModel.h++"
#include "vsArticulationGlove.h++"
#include "vsKinematics.h++"

class VS_MOTION_DLL vsHandArticulation : public vsMotionModel
{
protected:

    vsArticulationGlove    *glove;
    
    int                    numKinematics;
    vsKinematics           *handKin[VS_AG_NUM_JOINTS];

public:

                    vsHandArticulation(vsArticulationGlove *aGlove,
                                       int numKinematics,
                                       vsKinematics *handKinematics[]);
                                   
                    ~vsHandArticulation();
                
    const char *    getClassName();
    
    void            update();
};

#endif

